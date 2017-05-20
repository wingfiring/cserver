#ifndef CSERVER_DATA_H_
#define CSERVER_DATA_H_
#include <string>
#include <vector>
#include <cstdint>
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace csrv{
	//immeAPP
	struct motetx_t{
		float freq;
		std::string modu;
		std::string datr;
		std::string codr;	// optional
		bool adr;
	};
	struct userdata_t{
		uint32_t seqno;
		uint8_t port;
		std::string payload;
		motetx_t motetx;
	};

	struct gwrx_t{
		uint64_t eui;
		std::string time;
		bool timefromgateway;
		uint64_t chan;
		uint32_t rfch;
		float rssi;
		float lsnr;
	};

	struct app_t {
		uint64_t moteeui;
		std::string dir; //From AS to CS is “up” , otherwise is “dn”
		boost::optional<uint16_t> token;
		boost::optional<bool> confirmed;	// Type of the package, optional
		boost::optional<userdata_t> userdata;	// optional
		std::vector<gwrx_t> gwrx; //optional
	};

	struct mote_t{
		uint64_t eui;
		boost::optional<bool> seqnoreq;
		boost::optional<uint32_t> seqnogrant;
		boost::optional<bool> app;
		boost::optional<bool> sentStatus;
		boost::optional<uint16_t> msgsent;
		boost::optional<uint16_t> ackrx;

	};

	bool parse_json(app_t& ret, const std::string& json, const std::string& root);
	bool parse_json(mote_t& ret, const std::string& json, const std::string& root = "mote");
	bool base64_decode(const std::string& input, std::vector<char>& buf);
	std::string base64_encode(const char* buf, size_t n);
}
#endif //end CSERVER_DATA_H_
