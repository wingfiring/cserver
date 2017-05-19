#include <cserver/server.h>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/steady_timer.hpp>
//#include <boost/asio/ssl.hpp>

#include <boost/log/trivial.hpp>

#include <thread>
#include <iostream>
#include <string>
#include <array>
#include <chrono>
#include <iomanip>
#include <vector>
#include <sstream>

#include <cstdlib>

namespace csrv{
	namespace {
		const int K_retry_sleep_time = 5;
		const std::array<unsigned char, 15> K_CSHeart{
			0xb0, 0xb0, 0xb0,0xb0,	//head
				0xe0,				// command
				0x07,0x00,			//size
				'C','S','H','e','a','r','t',	// message
				0xf0				// end
		};
		const std::chrono::steady_clock::duration K_HeartbeatTimeout(std::chrono::seconds(10)); 

		const std::uint32_t K_startid=0xb0b0b0b0;

#pragma pack(push, 1)
		struct Header_t{
			std::uint32_t startid;
			std::uint8_t command;
			std::uint16_t length;	// little endian
		};
		union Header{
			Header_t head;
			std::uint8_t data[sizeof(Header_t)];
		};
		const size_t K_header_size = sizeof(Header_t);
#pragma pack(push)
	}

	class ServerImp{
		public:
			boost::asio::io_service ios_aserver, ios_client;
			boost::asio::io_service::work wk_aserver, wk_client;
			std::thread th_aserver, th_client;
			csrv::Config& config;
		public:
			ServerImp(csrv::Config& cfg) 
				: wk_aserver(ios_aserver)
				  , wk_client(ios_client)
				  , config(cfg)
		{}

			void start(){
				start_upstream_();
				start_console_();
				start_accept_();

				std::thread th_aserver([&]{ios_aserver.run();});
				std::thread th_client([&]{ios_client.run();});

				th_aserver.join();

				ios_client.stop();
				th_client.join();
			}

			void stop(){
				ios_aserver.stop();
				ios_client.stop();
			}

			void start_upstream_(){
				boost::asio::spawn(ios_aserver, [&](boost::asio::yield_context yield) { do_upstream_(yield);});
			}
			void start_console_(){
				boost::asio::spawn(ios_aserver, [&](boost::asio::yield_context yield) { do_console_(yield);});
			}
			void start_accept_(){
				boost::asio::spawn(ios_aserver, [&](boost::asio::yield_context yield) { do_accept_(yield);});
			}

			void do_upstream_(boost::asio::yield_context yield){
				using boost::asio::ip::tcp;
				tcp::resolver resolver(ios_aserver);
				boost::system::error_code ec;
				auto itr = resolver.async_resolve(tcp::resolver::query(config.aserver_site, config.aserver_port), yield[ec]);
				if (ec){
					BOOST_LOG_TRIVIAL(fatal) <<"Failed to resolve CServer site " << config.aserver_site;
					return;
				}

				for(;;){
					tcp::socket socket(ios_aserver);
					for (tcp::resolver::iterator end; itr != end; ++itr) {
						boost::asio::async_connect(socket, itr, yield[ec]);
						if (!ec) break;
					}
					if (ec || !socket.is_open()){
						BOOST_LOG_TRIVIAL(warning) <<"Failed to connect to CServer site" << config.aserver_site << ":" << config.aserver_port;
						sleep_for_(K_retry_sleep_time, yield);
						continue;
					}

					auto heatbeat_time = std::chrono::steady_clock::now();
					for(;;){
						if (heatbeat_time <= std::chrono::steady_clock::now()){
							boost::asio::async_write(socket, boost::asio::buffer(K_CSHeart), yield[ec]);	// heart beat
							if (ec){
								BOOST_LOG_TRIVIAL(warning) <<"Failed to send heartbeat.";
								break;
							}
							heatbeat_time += K_HeartbeatTimeout;
						}

						Header head;

						auto n = boost::asio::async_read(socket, boost::asio::buffer(head.data), yield[ec]);
						if (has_head_error_(n, head.head)) break;

						std::vector<char> buf(std::size_t(head.head.length) + 1);

						n = boost::asio::async_read(socket, boost::asio::buffer(buf), yield[ec]);

						if (n != buf.size()){
							BOOST_LOG_TRIVIAL(warning) <<"No enough data, lenght is " << head.head.length << ", but received only received " << n << " (+1) bytes";
							break;
						}

						if(buf.back() != 0xf0){
							BOOST_LOG_TRIVIAL(warning) <<"Bad end flag: 0x" << std::hex << buf.back();
							break;
						}

						buf.resize(buf.size() - 1);
						if (!parse_json_(buf)) break;

					}
					sleep_for_(K_retry_sleep_time, yield);
				}
			}
			void do_accept_(boost::asio::yield_context yield){

				//acceptor.reset(new tcp::acceptor(io_service, tcp::endpoint(boost::asio::ip::address::from_string(config.host.address.c_str()), config.host.port)));

			}
			void do_console_(boost::asio::yield_context yield){
				namespace posix = boost::asio::posix;
				posix::stream_descriptor input_(ios_aserver, ::dup(STDIN_FILENO));
				posix::stream_descriptor output_(ios_aserver, ::dup(STDOUT_FILENO));
				boost::asio::streambuf input_buffer_(1024);

				boost::system::error_code ec;
				for(;;){
					auto n = boost::asio::async_read_until(input_, input_buffer_, '\n', yield[ec]);
					std::vector<char> buf(n);
					input_buffer_.sgetn(&buf[0], n);
					buf.back() = 0;

					std::stringstream sstr(&buf[0]);
					std::string text;
					while(sstr >> text){
						if (text == "quit"){
							stop();
							return;
						}
					}
				}
			}

			void sleep_for_(int sec, boost::asio::yield_context yield){
				boost::asio::steady_timer timer(ios_aserver);
				timer.expires_from_now(std::chrono::seconds(sec));
				boost::system::error_code ec;
				timer.async_wait(yield[ec]);
			}
			bool has_head_error_(size_t n, const Header_t& head){
				if (n != K_header_size){
					BOOST_LOG_TRIVIAL(warning) <<"Failed to read data.";
					return true;
				}
				if (head.startid != K_startid){
					BOOST_LOG_TRIVIAL(warning) <<"Bad startid: 0x" << std::hex << head.startid;
					return true;
				}
				if(head.command != 0xe0 && head.command != 0xe1){
					BOOST_LOG_TRIVIAL(warning) <<"Bad command: 0x" << std::hex << head.command;
					return true;
				}
				return false;
			}
			bool parse_json_(const std::vector<char>& ){
				return true;

			}
	};

	Server::Server(csrv::Config& cfg)
		: m_imp(new ServerImp(cfg))
	{}
	Server::~Server(){}

	void Server::start(){ m_imp->start();}
	void Server::stop(){ m_imp->stop();}
}
