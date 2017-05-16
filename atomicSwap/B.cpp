#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_WalletLITECOIN.cpp"
#include <string>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

payment_address ltcBid(){
	script ltcContract = script();
	ltcContract.from_string("if hash160 [7e88c8277e78610110c79a77eb0d340fba0c2775] equal else [1495287154] checklocktimeverify drop dup hash160 [] equalverify checksig");
	return payment_address(ltcContract, 196);
}

point getUTXOs(payment_address Addy, std::string utxoString, int index)
{


	hash_digest utxo;
	decode_hash(utxo, utxoString);
	return  point(utxo, index);
	
	
	
	//return allPoints;
	


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
transaction payContract(HD_Wallet wallet, uint64_t amount)
{
	transaction tx = transaction();
	payment_address Addy = wallet.childAddress(2);


	points_value UTXOs = points_value();
	UTXOs.points = {point_value(getUTXOs(Addy, "", 0), 100000000)};
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

	script paymentScript(script().to_pay_script_hash_pattern(ltcBid().hash()));///SCRIPTHASH));
	output payment(amount, paymentScript);
	tx.outputs().push_back(payment);

	return sigScript(wallet.childPrivateKey(2).secret(), tx);


}

int main()
{
	HD_Wallet wallet(split("chase pair scorpion slab pause imitate dog blouse check dignity message strong"));
	std::cout << "Contract: " <<ltcBid().encoded() << std::endl;
	std::cout << "\n LTC seller pubkey: " << encode_base16(wallet.childPublicKey(1).point()) << std::endl;
	std::cout << "\nLTC bidder pubkey: " << encode_base16(wallet.childPublicKey(2).point()) << std::endl;
	std::cout <<"\n LTC bidder Address: " << wallet.childAddress(2).encoded() << std::endl;
	std::cout << "\n" << encode_base16(payContract(wallet, 100000000).to_data(1)) << std::endl;
}

