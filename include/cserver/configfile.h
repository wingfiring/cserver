#ifndef CSERVER_CONFIG_FILE_H
#define CSERVER_CONFIG_FILE_H
#include <string>

namespace csrv{

	struct Config{
		std::string config_path;

		std::string aserver_site, aserver_port;
		std::string cserver_addr, cserver_port;
		std::string log_file;	// default is /tmp/cserver.log

		int heartbeat;	// in seconds, default 10s
		int aserver_retry_sleep;	// sleep time on reading error from aserver, default 5s
		int log_level;

	};

	void loadConfig(Config& cfg, const std::string& path);
	void saveConfig(const Config& cfg, const std::string& path);

}

#endif //end CSERVER_CONFIG_FILE_H


