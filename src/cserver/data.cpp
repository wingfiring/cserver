#include <cserver/data.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <sstream>

namespace csrv{
	bool do_parse_json(app_t& ret, const std::string& json, const std::string& root){
		boost::property_tree::ptree pt;
		std::stringstream sstr(json);
		read_json(sstr, pt);

		auto itr = pt.find(root);
		if (itr == pt.not_found())
			return false;

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
			udata.port = unode.get<uint8_t>("port");
			udata.payload = unode.get<std::string>("payload");

			auto& mnode = node.get_child("motetx");
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

	bool parse_json(app_t& ret, const std::string& json, const std::string& root){
		try{
			return do_parse_json(ret, json, root);
		}catch(...){}
		return false;
	}
	bool parse_json(mote_t& ret, const std::string& json, const std::string& root){
		try{
			return do_parse_json(ret, json, root);
		}catch(...){}
		return false;
	}
}


