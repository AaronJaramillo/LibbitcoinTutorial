#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_WalletTESTNET.cpp"
#include <string>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;
//g++ -std=c++11 -o swap A.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin)

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

payment_address btcOffer(){
	script btcContract = script();
	std::string scriptString = "";
	btcContract.from_string("if hash160 [7e88c8277e78610110c79a77eb0d340fba0c2775] equal else [1496323954] checklocktimeverify drop dup hash160 [02e6705c1dfbaadb92254bc5958820feedd98de6859a321fdb4ea335db0a2b2c97] equalverify checksig");
	return payment_address(btcContract, 0xC4);
}

transaction payContract(HD_Wallet wallet, uint64_t amount)
{
	transaction tx = transaction();
	payment_address Addy = wallet.childAddress(1);


	points_value UTXOs = getUTXOs(Addy, amount + 1000);
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

	tx.set_inputs(inputs);

	uint64_t change = UTXOs.value() - amount - 1000;
	script outScript = script(script().to_pay_key_hash_pattern(Addy.hash()));
	std::cout << "change: "<< change << std::endl;
	tx.outputs().push_back(output(change, outScript));

	script paymentScript(script().to_pay_script_hash_pattern(btcOffer().hash()));///SCRIPTHASH));
	output payment(amount, paymentScript);
	tx.outputs().push_back(payment);

	return sigScript(wallet.childPrivateKey(1).secret(), tx);


}

int main()
{
	HD_Wallet wallet(split("chase pair scorpion slab pause imitate dog blouse check dignity message strong"));
	std::cout << "BTC seller pukbey: " << encode_base16(wallet.childPublicKey(1).point()) << "\n" << std::endl;
	std::cout << btcOffer().encoded() << "\n" <<std::endl;


	std::cout << "BTC bidder pubkey: " << encode_base16(wallet.childPublicKey(2).point()) << "\n" << std::endl;
	transaction payment = payContract(wallet, 100000000);
	std::cout << "\n" <<encode_base16(payment.to_data(1)) << "\n" <<std::endl;


}