#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

//ec_public takes base16
ec_public getPayer(std::string key){
	return ec_public(key);
}

ec_public getPayee(std::string key){
	return ec_public(key);
}

script makeRedeemScript(ec_public payer, ec_public payee){

	data_chunk p1 = to_chunk(payer.point());
	data_chunk p2 = to_chunk(payee.point());

	data_stack keys {p1, p2};
	script multisig = script(script().to_pay_multisig_pattern(2, keys));
	return multisig;

}

payment_address makeMultisigAddress(ec_public payer, ec_public payee){

	script redeem = makeRedeemScript(payer, payee);
	return payment_address(redeem, 0xc4);
}

transaction makeBond(){

	//Destination Address is the multisig addy 
	payment_address Destination = multisigAddy;
	script outputScript = script().to_pay_script_hash_pattern(multisigAddy.hash());
	//Amount to fill the channel with.
	uint64_t Satoshis; 
	string amount = "1";
	decode_base10(Satoshis, amount, 8);
	output output1(Satoshis, outputScript);

	///Get UTXOs

	//Previous Locking Script
	script prevLockingScript = script().to_pay_key_hash_pattern(bitcoin_short_hash(payer));

	//make Input


}

uint64_t getOutputs(payment_address address, uint64_t amount){

	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet.libbitcoin.net:19091");

	client::obelisk_client client(connection);

	points_value val1;
	static const auto on_done = [&val1](const points_value& vals) {

		std::cout << "Success: " << vals.value() << std::endl;
		val1 = vals;
		

	};

	static const auto on_error = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}

	client.blockchain_fetch_unspent_outputs(on_error, on_done, address, amount, select_outputs::algorithm::greedy);
	
	client.wait();
	
	
	//return allPoints;
	return val1.value();
}

output makeChange(uint64_t amount, uint64_t value, payment_address addy)
{
	if(value > amount)

	{
		uint64_t change = value - amount - 1000;
		script outScript = script(script().to_pay_key_hash_pattern(addy.hash()));
		
		std::cout << "change: "<< change << std::endl;
		return output(change, outScript);


	}
}

int main()
{
	//ec_public key1 = getPayer("");
	//ec_public key2 = getPayee("");


	payment_address addy = payment_address("n2ge1S4bLDvJKx8AGXrK5JHY2D5cReVytu");
	uint64_t amount = 100000000; 

	uint64_t value = getOutputs(addy, amount);
	std::cout << "value: " << value << std::endl;

	output change = makeChange(amount, value, addy);



}











