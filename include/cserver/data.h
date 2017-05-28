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
		uint32_t port;
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
#if 0
	Bytes	Type	Little	Signed	Description	Formula 
	1	INT	N/A	N	Light PWM (0-254)	L/254%
	4	INT	Y	N	Voltage (V)	V/1000
	4	INT	Y	N	Current (A)	C/1000
	4	INT	Y	N	Power (KW)	P/1000
	4	INT	Y	N	Energy (KWh)	E/1000
	2	INT	Y	Y	X-axis	X*0.00006G
	2	INT	Y	Y	Y-axis	Y*0.00006G
	2	INT	Y	Y	Z-axis	Z*0.00006G
	2	INT	Y	Y	Tilt angle	Tilt/10︒
	2	INT	Y	N	ALS Data(Lux)	A
	1	INT	N/A	N	Lighting Control Mode	M

#endif
	struct node_info_t{
		uint8_t	light_pwm;
		uint32_t voltage;
		uint32_t current;
		uint32_t power;
		uint32_t energy;
		int16_t x_axis;
		int16_t y_axis;
		int16_t z_axis;
		int16_t als;
		int8_t lcm;
		node_info_t();	// zero this
	};

	template<typename T>
	bool extract(const uint8_t*& data, size_t& len, T& t){
		if (len < sizeof(T))	return false;

		t = *reinterpret_cast<const T*>(data);
		data += sizeof(T);
		len -= sizeof(T);

		return true;
	}

	void parse_node_info(const uint8_t* data, size_t len, node_info_t& node);

	bool parse_json(app_t& ret, const std::string& json, std::string& app);
	bool parse_json(mote_t& ret, const std::string& json, const std::string& root = "mote");
	bool base64_decode(const std::string& input, std::vector<uint8_t>& buf);
	std::string base64_encode(const char* buf, size_t n);
}
#endif //end CSERVER_DATA_H_
