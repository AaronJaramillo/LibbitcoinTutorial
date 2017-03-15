#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>
#include <iostream>
#include <cstdint>

using namespace bc;
using namespace bc::chain;
using namespace bc::config;


int main()
{
	const auto& address = wallet::payment_address("1KvN5B9Q3qwgZ3iVpPCS4pxtYpZrBKmkSG");
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://mainnet.libbitcoin.net:9091");



	static const uint32_t timeout_ms = 3000;
	static const uint32_t retries = 3;

	const auto on_reply = [](size_t blockHeight) 
	{
		std::cout << "Height: " << blockHeight << std::endl;

	};

	static const auto on_error = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};
	static const auto on_unknown = [](const std::string& message) {
		std::cout << message << std::endl;
	};


// 	stream_fixture capture;
	client::obelisk_client client(connection);

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}


	static const auto on_done = [](const chain::history::list& rows){
		std::cout<< encode_base10(rows[0].value, 8) <<std::endl;

	};
	static const auto on_error2 = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};
	//client.connect(connection);
	client.blockchain_fetch_last_height(on_error, on_reply);
	client.wait();

	client.blockchain_fetch_history2(on_error2, on_done, address);
	client.wait();

	//client::proxy proxy(capture, on_unknown, timeout_ms, retries);
}
// 	//proxy.wait()
// 	//const auto height = source.read_string();
// 	//std::cout << height << std::endl;
// 	// client.wait();
// 	//std::cout << to_string(capture.out[0]) << std::endl;

