#ifndef CSERVER_SERVER_H_
#define CSERVER_SERVER_H_
#include <cserver/configfile.h>
#include <memory>

namespace csrv{

	class ServerImp;

	class Server{
		std::unique_ptr<ServerImp> m_imp;

		public:
		Server(csrv::Config& cfg);
		~Server();
		Server(Server&& rhs) = default;
		Server& operator=(Server&& rhs) = default;
		void swap(Server&& rhs){ rhs.m_imp.swap(m_imp); }

		void start();
		void stop();
	};

}

#endif //end CSERVER_SERVER_H_
