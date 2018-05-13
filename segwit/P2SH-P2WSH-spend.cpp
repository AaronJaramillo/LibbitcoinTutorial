#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>

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
void broadcastTX(transaction tx)
{
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet3.libbitcoin.net:19091");
	client::obelisk_client client(connection);

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}
	
	static const auto on_done = [](const code& ec) {

		std::cout << "Success: " << ec.message() << std::endl;

	};

	static const auto on_error2 = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	client.transaction_pool_broadcast(on_error2, on_done, tx);
	client.wait();
}
script getWitnessScript(ec_public key1, ec_public key2, ec_public key3) {
	//make key list
	point_list keys {key1.point(), key2.point(), key3.point()};
	//create 2/3 multisig script
	script multisig = script::to_pay_multisig_pattern(2u, keys);
	return multisig;
}

script getRedeemScript(ec_public key1, ec_public key2, ec_public key3) {
	//create 2/3 multisig script
	script multisig = getWitnessScript(key1, key2, key3);

	//sha256 the script
	data_chunk multisig_hash = to_chunk(sha256_hash(multisig.to_data(0)));

	//redeem script
	operation::list redeemscript_ops {operation(opcode(0)), operation(multisig_hash)};
	script redeem_script = script(redeemscript_ops);

	return redeem_script;
}

ec_private stringToPrivate(std::string stringEC){
	ec_secret secret;
	decode_base16(secret, stringEC);
	return ec_private(secret, ec_private::testnet);
}

int main() {

	ec_private key1 = stringToPrivate("e4ad55ecfd41062cec616762b73457c4d6bab35f3cb7dbe2fbab3f6a4d921bde");
	ec_private key2 = stringToPrivate("3edef3e2d2152156e6816c06e65f64baff4fc0831cf25ae86df016a5cb651747");
	ec_private key3 = stringToPrivate("5398432f87ad7b09d2aa6c3c2653d24a97b96fe1205d75f6d0dcf62b6c3414e1");

	payment_address fromAddress = payment_address(getRedeemScript(key1.to_public(), key2.to_public(), key3.to_public()), payment_address::testnet_p2sh);

	transaction tx;
	tx.set_version(1u);
	 //make output
	payment_address toAddress = wallet::payment_address("2N8hwP1WmJrFF5QWABn38y63uYLhnJYJYTF");
	uint64_t amount;
	btc_to_satoshi(amount, "0.64999");
	script outScript = script(script::to_pay_script_hash_pattern(toAddress));
	tx.outputs().push_back(output(amount, outScript));

	//make input
	points_value UTXOs = getUTXOs(fromAddress, amount);
	uint64_t previous_amount = UTXOs.points[0].value();

	input workingInput = input();
	workingInput.set_previous_output(output_point(UTXOs.points[0]));
	workingInput.set_sequence(max_input_sequence);
	tx.inputs().push_back(workingInput);


	script witness_script = getWitnessScript(key1.to_public(), key2.to_public(), key3.to_public());

	endorsement sig1;
	script().create_endorsement(sig1, key1.secret(), witness_script, tx, tx.inputs()[0].previous_output().index(), sighash_algorithm::all, script_version::zero, previous_amount);

	endorsement sig2;
	script().create_endorsement(sig2, key2.secret(), witness_script, tx, tx.inputs()[0].previous_output().index(), sighash_algorithm::all, script_version::zero, previous_amount);

	script redeem_script = getRedeemScript(key1.to_public(), key2.to_public(), key3.to_public());
	script inputScript = script(to_chunk(redeem_script.to_data(1)), 0);
	tx.inputs()[0].set_script(inputScript);

	data_chunk zero;
	data_stack witness_data {zero, sig1, sig2, witness_script.to_data(0)};
	tx.inputs()[0].set_witness(witness(witness_data));

	std::cout << "Raw TX: " << encode_base16(tx.to_data(1, 1)) << std::endl;
	broadcastTX(tx);
	//build
	//g++ -std=c++11 -o spend P2SH-P2WSH-spend.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin libbitcoin-client)


}








