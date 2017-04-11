#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>


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
transaction sigScript(ec_private wal, transaction tx)
{
	int index = 0;
	for (auto input: tx.inputs())
	{
		if(input.script() == script(script().to_pay_key_hash_pattern(wal.to_payment_address().hash())))
		{
			endorsement sig;
			script().create_endorsement(sig, wal.secret(), input.script(), tx, index, all_anyone_can_pay);
			operation::list ops {operation(sig), operation(to_chunk(wal.to_public().point()))};
			script scriptSig(ops);
			input.script().clear();
			input.set_script(scriptSig);
			tx.inputs()[index] = input;
		}
		index++;
	}
	return tx;

}

void broadcastTX(transaction tx)
{
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet1.libbitcoin.net:19091");
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
// output makeChange(uint64_t total, uint64_t amount, payment_address Addy)
// {
// 	uint64_t change = amount - total - 1000;
// 	script outScript = script(script().to_pay_key_hash_pattern(Addy.hash()));

// 	std::cout << "change: "<< change << std::endl;
// 	return output(change, outScript);
// }

output::list makeOuputs(uint64_t amount, payment_address address1, payment_address address2, payment_address address3)
{
	output output1(amount, script(script().to_pay_key_hash_pattern(address1.hash())));
	output output2(amount, script(script().to_pay_key_hash_pattern(address2.hash())));
	output output3(amount, script(script().to_pay_key_hash_pattern(address3.hash())));

	return output::list {output1, output2, output3};

}


int main()
{

	ec_private wal1(bitcoin_hash(decode_mnemonic(split(""))),0xEF6F, 1);
	ec_private wal2(bitcoin_hash(decode_mnemonic(split(""))),0xEF6F, 1);
	ec_private wal3(bitcoin_hash(decode_mnemonic(split(""))),0xEF6F, 1);

	std::cout << wal1.to_payment_address() << std::endl;
	std::cout << wal2.to_payment_address() << std::endl;
	std::cout << wal3.to_payment_address() << std::endl;

	payment_address destination1("n2ge1S4bLDvJKx8AGXrK5JHY2D5cReVytu");
	payment_address destination2("mnrnjVFimDFrNkszzMtecr4yrMKmEuMRbv");
	payment_address destination3("n2BPFTRKLtM6VQGN6tCWCaEbBuFTEo5P6r");

	transaction coinJoin = transaction();

	uint64_t amount = 100000000;
	output::list outputs = makeOuputs(amount, destination1, destination2, destination3);
	coinJoin.set_outputs(outputs);

	coinJoin = makeInput(wal1.to_payment_address(), coinJoin, amount);
	coinJoin = makeInput(wal2.to_payment_address(), coinJoin, amount);
	coinJoin = makeInput(wal3.to_payment_address(), coinJoin, amount);
	
	coinJoin = sigScript(wal1, coinJoin);
	coinJoin = sigScript(wal2, coinJoin);
	coinJoin = sigScript(wal3, coinJoin);

	std::cout << "\n" << encode_base16(coinJoin.to_data(1)) << std::endl;

	broadcastTX(coinJoin);





}



