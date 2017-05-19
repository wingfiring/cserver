#ifndef CSERVER_CONFIG_FILE_H
#define CSERVER_CONFIG_FILE_H
#include <string>

namespace csrv{

	struct Config{
		std::string config_path;

		std::string aserver_site, aserver_port;
		std::string cserver_addr, cserver_port;
	};

	void loadConfig(Config& cfg, const std::string& path);
	void saveConfig(const Config& cfg, const std::string& path);

}

#endif //end CSERVER_CONFIG_FILE_H


