#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>
#include <iostream>
#include <cstdint>

using namespace bc;

int main()
{
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

	static const auto on_error = [](const code& ec) 
	{

		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	client::obelisk_client client(connection);

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}

	client.blockchain_fetch_last_height(on_error, on_reply);
	client.wait();
}