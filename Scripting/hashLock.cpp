#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_WalletTESTNET.cpp"


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

points_value getUTXOs(payment_address Addy, uint64_t amount)
{
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet1.libbitcoin.net:19091");
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

	client.blockchain_fetch_unspent_outputs(on_error, on_done, Addy, amount, select_outputs::algorithm::greedy);
	
	client.wait();
	
	
	//return allPoints;
	return val1;


}

transaction makeInput(payment_address Addy, transaction tx, uint64_t amount)
{
	points_value UTXOs = getUTXOs(Addy, amount);
	input::list inputs {};
	
	for (auto utxo: UTXOs.points)
	{
		input workingInput = input();
		workingInput.set_previous_output(output_point(utxo));

		std::cout << encode_base16(utxo.hash()) << std::endl;
		workingInput.set_script(script(script().to_pay_key_hash_pattern(Addy.hash())));
		workingInput.set_sequence(0xffffffff);
		inputs.push_back(workingInput);
	}

	uint64_t change = UTXOs.value() - amount - 1000;
	script outScript = script(script().to_pay_key_hash_pattern(Addy.hash()));

	std::cout << "change: "<< change << std::endl;
	tx.outputs().push_back(output(change, outScript));
	
	extend_data(tx.inputs(), inputs);
	return tx;
}

int main()
{
	HD_Wallet wallet(split("chase pair scorpion slab pause imitate dog blouse check dignity message strong")); 
	transaction hashLock = transaction();
	//hashLock = makeInput(wallet.childAddress(1), hashLock, 100000000);

	hash_digest utxoHash;
	decode_hash(utxoHash, "fddde55c869ec2ae040f37dc55790f3f2ce2e762a9ca8670f799c057d7ee3ecc");
	input input1 = input();
	input1.set_previous_output(output_point(utxoHash, 0));

	script redeemScript = script().from_string("dup hash160 [7e88c8277e78610110c79a77eb0d340fba0c2775] equal")
	script lockingScript = bitcoin_hash_chunk(redeemScript.to_data(0));
	output output1();

	std::cout << encode_base16(hashLock.to_data(1)) << std::endl;


}



