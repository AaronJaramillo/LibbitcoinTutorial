#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_WalletTESTNET.cpp"


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

transaction spendRefund(transaction contract, script redeemScript, HD_Wallet wallet)
{
	payment_address withdrawAddress("n2ge1S4bLDvJKx8AGXrK5JHY2D5cReVytu");
	transaction spendContract = transaction();
	spendContract.set_version(1);


	hash_digest utxoHash;
	decode_hash(utxoHash, "51ddb354a43b0e5c4ee861eb5baa0b79cdb8d495dd7276d0cfa0e7bdf46a9e50");
	output_point contractUTXO(utxoHash, 0);
	
	data_chunk preImage;
	decode_base16(preImage, "4c6962626974636f696e");


	 //({operation(preImage), operation(redeemScript.to_data(0))});
	//unlockingScript.from_string("[4c6962626974636f696e] [6376a914e82f89457f9efaab09b3222b5f7f82b4ab82683288ac67a9147e88c8277e78610110c79a77eb0d340fba0c277587]");



	input input1 = input();
	input1.set_previous_output(contractUTXO);

	script lockingScript(script().to_pay_key_hash_pattern(withdrawAddress.hash()));
	output output2 = output(299993999, lockingScript);

	spendContract.inputs().push_back(input1);
	spendContract.outputs().push_back(output2);

	endorsement sig;
	script().create_endorsement(sig, wallet.childPrivateKey(1).secret(), redeemScript, spendContract, 0, all);
	script unlockingScript = script({operation(sig), operation(to_chunk(wallet.childPublicKey(1).point())), operation(redeemScript.to_data(1))});

	spendContract.inputs()[0].set_script(unlockingScript);

	return spendContract;

}

transaction resolveContract(transaction contract, script redeemScript)
{
	payment_address withdrawAddress("mtawgig12q69tMuppx2xaQVhBHP1nuHZaA");
	transaction spendContract = transaction();
	spendContract.set_version(1);


	hash_digest utxoHash;
	decode_hash(utxoHash, "51ddb354a43b0e5c4ee861eb5baa0b79cdb8d495dd7276d0cfa0e7bdf46a9e50");
	output_point contractUTXO(utxoHash, 0);
	
	data_chunk preImage;
	decode_base16(preImage, "4c6962626974636f696e");

	script unlockingScript({operation(preImage), operation(opcode(0)), operation(redeemScript.to_data(0))});
	//unlockingScript.from_string("[4c6962626974636f696e] [0] [6376a914e82f89457f9efaab09b3222b5f7f82b4ab82683288ac67a9147e88c8277e78610110c79a77eb0d340fba0c277587]");



	input input1(contractUTXO, unlockingScript, 0xffffffff);

	script lockingScript(script().to_pay_key_hash_pattern(withdrawAddress.hash()));
	output output2 = output(299993999, lockingScript);

	spendContract.inputs().push_back(input1);
	spendContract.outputs().push_back(output2);

	return spendContract;
}

int main()
{
	HD_Wallet wallet(split("chase pair scorpion slab pause imitate dog blouse check dignity message strong")); 
	transaction hashLock = transaction();
	
	hash_digest utxoHash;
	decode_hash(utxoHash, "fddde55c869ec2ae040f37dc55790f3f2ce2e762a9ca8670f799c057d7ee3ecc");
	input input1 = input();
	input1.set_previous_output(output_point(utxoHash, 0));

	script redeemScript = script();
	redeemScript.from_string("if dup hash160 [e82f89457f9efaab09b3222b5f7f82b4ab826832] equalverify checksig else hash160 [7e88c8277e78610110c79a77eb0d340fba0c2775] equal");
	payment_address contractAddress(redeemScript, 0xC4);
	std::cout << "\n" << contractAddress.encoded() << "\n"<< std::endl;
	script lockingScript = script(script().to_pay_script_hash_pattern(contractAddress.hash()));
	output output1(299996999, lockingScript);
	hashLock.inputs().push_back(input1);
	hashLock.outputs().push_back(output1);

	endorsement sig;
	script().create_endorsement(sig, wallet.childPrivateKey(1).secret(), script(script().to_pay_key_hash_pattern(wallet.childAddress(1).hash())), hashLock, 0, all);
	operation::list ops = {operation(sig), operation(to_chunk(wallet.childPublicKey(1).point()))};
	hashLock.inputs()[0].set_script(script(ops));
	std::cout << encode_base16(hashLock.to_data(1)) << std::endl;


	std::cout << "\n" << encode_base16(resolveContract(hashLock, redeemScript).to_data(1)) << std::endl;

	std::cout << "\n" << encode_base16(spendRefund(hashLock, redeemScript, wallet).to_data(1)) << std::endl;




}



