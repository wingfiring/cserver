#ifndef CSERVER_DB_H_
#define CSERVER_DB_H_
#include <cserver/configfile.h>
#include <cserver/data.h>
#include <memory>

namespace csrv{

	class DatabaseImp;
	class Database{
		std::unique_ptr<DatabaseImp> m_imp;
		public:

		explicit Database(const Config&);
		~Database();

		void saveDeviceRecord(const app_t&);

		void writeTest(const std::string& str);
	};
	
}
#endif //end CSERVER_DB_H_
