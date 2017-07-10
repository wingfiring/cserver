#include <cserver/db.h>
#include <cserver/data.h>
#include <boost/log/trivial.hpp>
#include <iomanip>
#include <iostream>
#include <exception>

#include <pqxx/connection.hxx>
#include <pqxx/transaction.hxx>
namespace csrv{

	class DatabaseImp{
		public:
			const Config& config;
			pqxx::connection dbconn;
			DatabaseImp(const Config& cfg) 
				: config(cfg)
				, dbconn(cfg.db_string.c_str())
			{
				dbconn.prepare("insert_device", "INSERT INTO t_device(moteeui, gweui, seqno, rfchan, chan, freq, modu, datr, adr, sf, rssi, snr, "
						"light_pwm,voltage,current,power,energy,x_axis,y_axis,z_axis,als,lcm)"
						" values($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19,$20,$21,$22)");
				dbconn.prepare("write_test", "INSERT INTO test(text) values($1)");
				dbconn.prepare("insert_node_data", 
						"insert into t_node_data(euid, log_time, rtime, rtime_from_gw, gweuid, seqno, light_pwm, voltage, current, power, energy) "
						"values($1, CURRENT_TIMESTAMP, $2, $3, $4, $5, $6, $7, $8, $9, $10)");

			}
			void writeTest(const std::string& str){
				pqxx::work w(dbconn);
				w.prepared("write_test")(str).exec();
				w.commit();
			}

			void saveDeviceRecord(const app_t& app){
				if (app.userdata && !app.gwrx.empty()){
					auto& udata = *app.userdata;
					std::vector<uint8_t> buf;
					if (base64_decode(udata.payload, buf)){
						std::stringstream sstr;
						sstr << "Payload (size=" << buf.size() << "):[";
						for(auto ch : buf)
							sstr << ' ' << std::setfill('0') << std::setw(2) << std::hex << uint32_t(ch);
						sstr << " ]";
						if (!buf.empty() && *buf.begin() == 0x02){
							BOOST_LOG_TRIVIAL(info) << sstr.str();
							node_info_t node;
							if(parse_payload(buf, node)){
								auto& gwrx = app.gwrx[0];
								try{

									BOOST_LOG_TRIVIAL(info) 
										<< " time:" << gwrx.time 
										<< " moteeui:" << (app.moteeui) 
										<< " gwrx.eui:" << (gwrx.eui) << " seqno:" << (udata.seqno)
										<< " rfch:" << (gwrx.rfch) << " chan:" << (gwrx.chan)
										<< " freq:" << (udata.motetx.freq) << " modu:" << (udata.motetx.modu)
										<< " datr:" << (udata.motetx.datr)  << " adr:" << (udata.motetx.adr)
										<< " port:" << (udata.port)
										<< " rssi:" << (gwrx.rssi) << " lsnr:" << (gwrx.lsnr)
										<< " light_pwm:" << (int32_t(node.light_pwm)) << " voltage:" << (node.voltage)
										<< " current:" << (node.current) << " power:" << (node.power)
										<< " energy:" << (node.energy) << " x_axis:" << (node.x_axis)
										<< " y_axis:" << (node.y_axis) << " z_axis:" << (node.z_axis)
										<< " als:" << (node.als) << " lcm:" << (int32_t(node.lcm));
#if 1
									pqxx::work w(dbconn);
									w.prepared("insert_node_data")
										(app.moteeui)
										(gwrx.time)
										(gwrx.timefromgateway)
										(gwrx.eui)
										(udata.seqno)
										(int32_t(node.light_pwm))
										(node.voltage)
										(node.current)
										(node.power)
										(node.energy)
										.exec();
									w.commit();
#endif
								}
								catch(std::exception& e){
									BOOST_LOG_TRIVIAL(warning) << "Failed to insert db: " << e.what();
								}
								catch(...){
									BOOST_LOG_TRIVIAL(warning) << "Failed to insert db";
								}
							}
							else 
								BOOST_LOG_TRIVIAL(debug) << "Failed to parse payload";
						}
						else
							BOOST_LOG_TRIVIAL(debug) << sstr.str();
					}
					else
						BOOST_LOG_TRIVIAL(warning) <<"Bad base64 encoded payload: " << udata.payload;
				}
				else {
					BOOST_LOG_TRIVIAL(warning) <<"No userdata.";
				}
			}
			bool parse_payload(const std::vector<uint8_t>& data, node_info_t& node){
				auto end = data.end();
				for(auto itr =  data.begin(); itr != end;){
					auto cmd = *itr;
					if (cmd != 0x02 || ++itr == end) break;
					auto len = *itr++;
					if (len > (end - itr))	break;	// no enough value data

					parse_node_info(&*itr, len, node);
					return true;
				}
				return false;
			}

			void print_node(const node_info_t& node){
				std::stringstream sstr;
				sstr << "node_info:{";

				sstr 
					<< " light_pwm:" << uint32_t(node.light_pwm)
					<< " voltage:" << uint32_t(node.voltage)
					<< " current:" << uint32_t(node.current)
					<< " power:" << uint32_t(node.power)
					<< " energy:" << uint32_t(node.energy)
					<< " x-axis:" << int32_t(node.x_axis)
					<< " y-axis:" << int32_t(node.y_axis)
					<< " z-axis:" << int32_t(node.z_axis)
					<< " als:" << int32_t(node.als)
					<< " lcm:" << int32_t(node.lcm)

					<< " }";
				BOOST_LOG_TRIVIAL(info) << sstr.str();
			}
	};

	Database::Database(const Config& cfg)
	{
		try{
			m_imp.reset(new DatabaseImp(cfg));
		} catch(std::exception& e){
			std::cout << "Database ctor: " << e.what() << std::endl;
			throw;

		}
	}
	Database::~Database() = default;

	void Database::saveDeviceRecord(const app_t& app){
		m_imp->saveDeviceRecord(app);
	}
	void Database::writeTest(const std::string& str){
		m_imp->writeTest(str);
	}


}

