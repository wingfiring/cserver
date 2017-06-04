#include <cserver/db.h>
#include <cserver/data.h>
#include <boost/log/trivial.hpp>
#include <iomanip>
#include <iostream>

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
									pqxx::work(dbconn);
									w.prepared("insert_device")
										(app.moteeui)(gwrx.eui)(udata.seqno)
										(gwrx.rfch)(gwrx.chan)
										(udata.motetx.freq)(udata.motetx.modu)
										(udata.motetx.datr) (udata.motetx.adr)
										(udata.port)
										(gwrx.rssi)(gwrx.lsnr)
										(int32_t(node.light_pwm))(node.voltage)
										(node.current)(node.power)
										(node.energy)(node.x_axis)
										(node.y_axis)(node.z_axis)
										(node.als)(int32_t(node.lcm))
										.exec();
									w.commit();
								}catch(...){
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

