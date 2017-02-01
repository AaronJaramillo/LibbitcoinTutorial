#include <bitcoin/bitcoin.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;



int main()
{
	std::string Mnemonic1 = "chase pair scorpion slab pause imitate dog blouse check dignity message strong";
	std::string Mnemonic2 = "logic waste merit drama fatal pen type embody room ladder skin chicken";
	std::string Mnemonic3 = "early total frog beauty shrimp urban waste detect journey dad draft myth";

	HD_Wallet wallet1(split(Mnemonic1));
	HD_Wallet wallet2(split(Mnemonic2));
	HD_Wallet wallet3(split(Mnemonic3));

	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).point());
	data_chunk pubkey2 = to_chunk(wallet2.childPublicKey(1).point());
	data_chunk pubkey3 = to_chunk(wallet3.childPublicKey(1).point());
	wallet1.displayAddress(1);



	operation::list opList {operation(opcode(82)), operation(opcode(33)), operation(pubkey1), operation(opcode(33)), operation(pubkey2), operation(opcode(33)), operation(pubkey3), operation(opcode(83)), operation(opcode(175))};
	script multisigScript(opList);

	if(multisigScript.is_valid())
	{
		std::cout << "Script is Valid!\n" << std::endl;
	}else{
		std::cout << "Script Invalid!  \n" << std::endl;
	}

	std::cout << "Redeeem Script: \n" << std::endl;
	std::cout << multisigScript.to_string(0) << "\n" << std::endl;
	std::cout << encode_base16(multisigScript.to_data(0)) << "\n" <<std::endl;

	short_hash scriptHash = bitcoin_short_hash(multisigScript.to_data(0));

	script pay2ScriptHash = script(multisigScript.to_pay_script_hash_pattern(scriptHash));



	std::cout << "Locking Script: " << std::endl;
	std::cout << pay2ScriptHash.to_string(0) << "\n" << std::endl;

	payment_address multsigAddress(multisigScript, 0xc4);
	std::cout << "Payment Address: " << std::endl;

	std::cout << multsigAddress.encoded() << "\n" << std::endl;
	std::cout << payment_address(scriptHash, 0xc4p).encoded() << "\n" << std::endl;

	// std::cout << "Private Key Mnemonics: \n" << std::endl;
	// std::cout << "Key One: " << wallet1.displayMnemonic() << std::endl;
	// std::cout << "Key Two: " << wallet2.displayMnemonic() << std::endl;
	// std::cout << "Key Three: " << wallet3.displayMnemonic() << "\n" <<std::endl; 
	//3C9SjwS39LrmsQtN9VwrP5rJkao4h5Tu4T


}