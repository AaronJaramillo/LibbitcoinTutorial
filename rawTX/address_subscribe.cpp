#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>
#include <iostream>

using namespace bc;

void getTXInfo(client::connection_type connection, const hash_digest tx_hash, wallet::payment_address watch_address) {
	client::obelisk_client client(connection);

	auto on_done = [&watch_address](const chain::transaction tx)
	{
		for (chain::output out: tx.outputs())
		{	
			// std::cout << wallet::payment_address(out.address().hash(), wallet::payment_address::testnet_p2kh).encoded() <<std::endl;
			if (wallet::payment_address(out.address().hash(), wallet::payment_address::testnet_p2kh) == watch_address)
				std::cout << "Transaction value: "<< encode_base10(out.value(), 8) << std::endl;

		}
	};

	auto on_error = [](const code& ec)
	{
		std::cout << "Error Code: " << ec.message() << std::endl;
	};

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	}

	client.transaction_pool_fetch_transaction(on_error, on_done, tx_hash);
	client.wait();
}

void subscribe(wallet::payment_address watch_address) {

	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet3.libbitcoin.net:19091");
	//instantiate obelisk client
	client::obelisk_client client(connection);

	static const auto on_subscribed = [&watch_address](const code& ec) {
		std::cout << "Subscription to " << watch_address.encoded() <<": " << ec.message() << std::endl;
	};

	static const auto on_error = [](const code ec) {
		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	auto on_update = [&connection, &watch_address](const code& error, uint16_t sequence, size_t height, const hash_digest& tx_hash)
	{
		std::cout << "New Transaction Recieved: " << std::endl;
		std::cout << sequence <<": "<< "Hash: " << encode_hash(tx_hash) <<" Confirmed: "<< height << std::endl;
		getTXInfo(connection, tx_hash, watch_address);
	};

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}

	client.set_on_update(on_update);
	client.subscribe_address(on_error, on_subscribed, watch_address);
	client.wait();
	client.monitor(8000); //in seconds

}

int main() {
	subscribe(wallet::payment_address("mnrnjVFimDFrNkszzMtecr4yrMKmEuMRbv"));
}