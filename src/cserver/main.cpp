
#include <cserver/configfile.h>
#include <cserver/log.h>
#include <cserver/server.h>

#include <boost/log/trivial.hpp>

#include <iostream>
#include <string>

int main(){
	csrv::Config config;
	csrv::Server server(config);
	csrv::init_log("/tmp/cserver", boost::log::trivial::severity_level::info);

	server.start();

	std::string text;

	while((std::cin >> text)){
		if (text == "quit")
			break;
	};
	server.stop();

	std::cout << "Bye.\n";
}
