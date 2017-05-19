#include <cserver/log.h>
#include <boost/log/trivial.hpp>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>

namespace csrv{
	namespace logging = boost::log;
	struct severity_tag;

	logging::formatting_ostream& operator<< (
		 logging::formatting_ostream& strm,
		 logging::to_log_manip< boost::log::trivial::severity_level, severity_tag > const& manip
		 ) {
		static const char* strings[] =
		{
			"trace",
			"debug",
			"info",
			"warning",
			"error",
			"fatal"
		};

		boost::log::trivial::severity_level level = manip.get();
		if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
			strm << strings[level];
		else
			strm << static_cast< int >(level);

		return strm;
	}

	void init_log(const std::string& path, int level_)
	{	 
		auto level = boost::log::trivial::severity_level(level_);

		namespace sinks = boost::log::sinks;
		namespace src = boost::log::sources;
		namespace expr = boost::log::expressions;
		namespace attrs = boost::log::attributes;
		namespace keywords = boost::log::keywords;
		logging::add_common_attributes();

		auto format = (expr::stream 
				<<  expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "[%Y-%m-%d %H:%M:%S.%f] [") 
				<< expr::attr< boost::log::trivial::severity_level, severity_tag >("Severity")  << "]: " 
				<< expr::smessage);

		logging::add_console_log(std::clog, keywords::format = format);

		logging::add_file_log
			(
			 keywords::file_name = path,                                        
			 keywords::rotation_size = 10 * 1024 * 1024,                                   
			 keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), 
			 keywords::format = format,
			 keywords::auto_flush = true,
			 keywords::open_mode = std::ios::app
			);

		logging::core::get()->set_filter
			(
			 logging::trivial::severity >= level
			);
	}

}
