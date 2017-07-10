#include <cserver/data.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <iostream>

namespace csrv{
	static const std::string app("app");
	static const std::string immeApp("immeAPP");
	bool do_parse_json(app_t& ret, const std::string& json, std::string& which_app){
		boost::property_tree::ptree pt;
		std::stringstream sstr(json);
		read_json(sstr, pt);

		auto itr = pt.find(immeApp);
		if (itr == pt.not_found()){
			itr = pt.find(app);
			if (itr == pt.not_found())
				return false;
			which_app = app;
		}
		else
			which_app = immeApp;

		auto& node = itr->second;
		ret.moteeui = node.get<uint64_t>("moteeui");
		ret.dir = node.get<std::string>("dir");
		ret.token = node.get_optional<uint16_t>("token");
		ret.confirmed = node.get_optional<bool>("confirmed");

		auto itr_userdata = node.find("userdata");
		if (itr_userdata != node.not_found()){
			auto& unode = itr_userdata->second;
			ret.userdata = userdata_t{};
			auto& udata = *ret.userdata;
			udata.seqno = unode.get<uint32_t>("seqno");
			udata.port = unode.get<uint32_t>("port");
			udata.payload = unode.get<std::string>("payload");

			auto& mnode = unode.get_child("motetx");
			auto& motetx = udata.motetx;
			motetx.freq = mnode.get<float>("freq");
			motetx.modu = mnode.get<std::string>("modu");
			motetx.datr = mnode.get<std::string>("datr");
			motetx.codr = mnode.get<std::string>("codr", "");
			motetx.adr = mnode.get<bool>("adr");
		}

		auto itr_gwrx = node.find("gwrx");
		if (itr_gwrx != node.not_found()){
			for(auto& item : itr_gwrx->second){
				gwrx_t gwrx;
				auto& gnode = item.second;

				gwrx.eui = gnode.get<uint64_t>("eui");
				gwrx.time = gnode.get<std::string>("time");
				std::replace(gwrx.time.begin(), gwrx.time.end(), 'T', ' ');
				std::replace(gwrx.time.begin(), gwrx.time.end(), 'Z', ' ');
				gwrx.timefromgateway = gnode.get<bool>("timefromgateway");
				gwrx.chan = gnode.get<uint64_t>("chan");
				gwrx.rfch = gnode.get<uint32_t>("rfch");
				gwrx.rssi = gnode.get<float>("rssi");
				gwrx.lsnr = gnode.get<float>("lsnr");

				ret.gwrx.emplace_back(std::move(gwrx));
			}
		}

		return true;
	}

	bool do_parse_json(mote_t& ret, const std::string& json, const std::string& root){
		boost::property_tree::ptree pt;
		std::stringstream sstr(json);
		read_json(sstr, pt);

		auto itr = pt.find(root);
		if (itr == pt.not_found())
			return false;

		auto& node = itr->second;
		ret.eui = node.get<uint64_t>("eui");
		ret.seqnoreq = node.get_optional<bool>("seqnoreq");
		ret.seqnogrant = node.get_optional<uint32_t>("seqnogrant");
		ret.app = node.get_optional<bool>("app");
		ret.sentStatus = node.get_optional<bool>("sentStatus");
		ret.msgsent = node.get_optional<uint16_t>("msgsent");
		ret.ackrx = node.get_optional<uint16_t>("ackrx");
		return true;
	}

	node_info_t::node_info_t(){
		memset(this, 0, sizeof(*this));
	}
	void parse_node_info(const uint8_t* data, size_t len, node_info_t& node){
		extract(data, len, node.voltage);
		extract(data, len, node.current);
		extract(data, len, node.power);
		extract(data, len, node.energy);
	}
	bool parse_json(app_t& ret, const std::string& json, std::string& app){

		try{
			return do_parse_json(ret, json, app);
		}catch(...){}
		return false;
	}
	bool parse_json(mote_t& ret, const std::string& json, const std::string& root){
		try{
			return do_parse_json(ret, json, root);
		}catch(...){}
		return false;
	}

	bool base64_decode(const std::string& input, std::vector<uint8_t>& buf){
		using namespace boost::archive::iterators;
		using It =  transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;  
		try {  
			std::copy(It(input.begin()), It(input.end()), std::back_inserter(buf));  
			return true;
		} catch(...) {  
		}  
		return false;  
	}
	std::string base64_encode(const char* buf, size_t n){
		using namespace boost::archive::iterators;
		using It = base64_from_binary<transform_width<const char*, 6, 8> >;  

		std::string result;
		std::copy(It(buf) , It(buf + n), std::back_inserter(result));  

		auto equal_count = (3 - n % 3) % 3;
		result.resize(result.size() + equal_count, '=');

		return result;
	}
}


