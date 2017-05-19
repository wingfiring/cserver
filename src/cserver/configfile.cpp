#include <cserver/configfile.h>

#include <fstream>
#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace csrv{

	void loadConfig(Config& cfg, const std::string& path){
		std::ifstream sstr(path);

		using boost::property_tree::ptree;
		ptree pt;
		read_info(sstr, pt);

		cfg.config_path = path;

		cfg.aserver_site = pt.get<std::string>("aserver_site", "");
		cfg.aserver_port = pt.get<std::string>("aserver_port", "");
		cfg.cserver_addr = pt.get<std::string>("cserver_addr", "0.0.0.0");
		cfg.cserver_port = pt.get<std::string>("cserver_port", "8000");

		cfg.log_file = pt.get<std::string>("log_file", "/tmp/cserver.log");

		cfg.heartbeat = pt.get<int>("heartbeat", 10);
		cfg.aserver_retry_sleep = pt.get<int>("aserver_site", 5);
	}

	void saveConfig(const Config& cfg, const std::string& path){
		boost::property_tree::ptree pt;
		
		pt.put("aserver_site", cfg.aserver_site);
		pt.put("aserver_port", cfg.aserver_port);
		pt.put("cserver_addr", cfg.cserver_addr);
		pt.put("cserver_port", cfg.cserver_port);
		pt.put("log_file", cfg.log_file);
		pt.put("heartbeat", cfg.heartbeat);
		pt.put("aserver_retry_sleep", cfg.aserver_retry_sleep);
	}
}
