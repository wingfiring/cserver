
#include <cserver/configfile.h>
#include <cserver/log.h>
#include <cserver/server.h>

#include <boost/log/trivial.hpp>

#include <iostream>
#include <string>


int main(int argc, char** argv){
	if (argc != 2){
		std::cout << "Usage cserver <config file>\n";
		return 1;
	}
	csrv::Config config;
	csrv::loadConfig(config, argv[1]);
	csrv::Server server(config);
	csrv::init_log("/tmp/cserver.log", boost::log::trivial::severity_level::info);

	server.start();

	std::cout << "Bye.\n";
}
