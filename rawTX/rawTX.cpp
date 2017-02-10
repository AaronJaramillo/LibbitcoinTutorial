#include <bitcoin/bitcoin.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

std::string getInput2()
{
	std::string input; 
	//cin.ignore();
	getline(cin, input);
	return input;
}
//This is a testing class
//use it to pass hardcoded strings
//to the prompts by changing the string
//declarations function and passing the preset
std::string getInput(int preset)
{
	if(preset == 1)
	{
		
		return ""; //Mnemonic
	} else if (preset == 2)
	{
		return "1"; //Index of child key
	}else if (preset == 3)
	{

		return ""; //Destination Adress
	}else if (preset == 4)
	{
		return "1.482947"; //Amount of Bitcoin to Spend
	} else if (preset == 5)
	{
		return ""; //UTXO hash to spend

	}else if (preset == 6)
	{
		return "0"; //Output index of UTXO
	}else{
		return getInput2();
	}
}

//Go here to get testnet coins https://testnet.manu.backend.hamburg/faucet
int main() 
{
	std::cout << "Import Wallet Via Mnemonic: " << std::endl;
	std::string Mnemonic1 = getInput(1);
	std::cout <<"\nChild Index To Spend From: " << std::endl;
	int child = atoi(getInput(2).c_str());
	HD_Wallet wallet1(split(Mnemonic1));
	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(child).point());

	//Make Output//
	std::cout << "\nEnter Destination Address: " << std::endl;
	std::string Destination = getInput(3); 
	payment_address destinationAddy(Destination);
	script outputScript = script().to_pay_script_hash_pattern(destinationAddy.hash());

	std::cout << "\nEnter Amount(BTC) To Send: " << std::endl;
	std::string BTC = getInput(4);
	uint64_t Satoshis;
	decode_base10(Satoshis, BTC, 8);
	output output1(Satoshis, outputScript);
	std::cout << "\nAmount: " << encode_base10(output1.value(), 8) << "BTC : Output Script: " << output1.script().to_string(0) << "\n" << std::endl;

	//Get UTXO
	std::cout << "\nEnter UTXO Hash To Spend: " << std::endl;
	std::string hashString = getInput(5); 
	std::cout << "\nEnter Output Index: " << std::endl;
	std::string index = getInput(6);
	uint32_t index1 = atoi(index.c_str());
	hash_digest utxoHash;  
	decode_hash(utxoHash, hashString);
	output_point utxo(utxoHash, index1);

	//Previous Locking Script
	script lockingScript = script().to_pay_key_hash_pattern(bitcoin_short_hash(pubkey1));

	//make Input
	input input1 = input();
	input1.set_previous_output(utxo);
	input1.set_sequence(0xffffffff);

	std::cout << "\nPrevious Locking Script: " << std::endl;
	std::cout << lockingScript.to_string(0xffffffff) << "\n" << std::endl;

	//build TX
	transaction tx = transaction();
	tx.inputs().push_back(input1);
	tx.outputs().push_back(output1);

	//Endorse TX
	endorsement sig; 
	if(lockingScript.create_endorsement(sig, wallet1.childPrivateKey(child).secret(), lockingScript, tx, 0u, all))
	{
		std::cout << "Signature: " << std::endl;
		std::cout << encode_base16(sig) << "\n" << std::endl; 
	}

	//make Sig Script
	operation::list sigScript; 
	sigScript.push_back(operation(sig));
	sigScript.push_back(operation(pubkey1));
	script unlockingScript(sigScript);
	std::cout << unlockingScript.to_string(0xffffffff) << "\n" << std:: endl;

	//Make Signed TX
	tx.inputs()[0].set_script(unlockingScript);
	std::cout << "Raw Transaction: " << std::endl;
	std::cout << encode_base16(tx.to_data()) << std::endl;
	//Copy and paste the encoded transaction to this website 
	//https://live.blockcypher.com/btc-testnet/pushtx/
	//to broadcast it to the testnet blockchain.












}