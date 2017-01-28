#include <bitcoin/bitcoin.hpp>
#include "HD_wallet.cpp"


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;



int main()
{
	HD_Wallet wallet1 = HD_Wallet();
	HD_Wallet wallet2 = HD_Wallet();
	HD_Wallet wallet3 = HD_Wallet();

	hd_key pubkey1 = wallet1.childPublicKey(1).to_hd_key();
	hd_key pubkey2 = wallet2.childPublicKey(1).to_hd_key();
	hd_key pubkey3 = wallet3.childPublicKey(1).to_hd_key();

	data_chunk m;
	decode_base16(m, "2");
	data_chunk n;
	decode_base16(n, "3");
	operation multisigOP(opcode(174));
	data_chunk rawScript = build_chunk({m, pubkey1, pubkey2, pubkey3, n, multisigOP.to_data()});

	script multisigScript(rawScript, false);


	std::cout << "Redeeem Script: \n" << std::endl;
	std::cout << multisigScript.to_string(0) << "\n" << std::endl;

	short_hash scriptHash = bitcoin_short_hash(rawScript);

	multisigScript = script(multisigScript.to_pay_script_hash_pattern(scriptHash));

	if(multisigScript.is_valid())
	{
		std::cout << "Script is Valid!\n" << std::endl;
	}
	std::cout << "Locking Script: " << std::endl;
	std::cout << multisigScript.to_string(0) << "\n" << std::endl;

	payment_address multsigAddress(multisigScript);
	std::cout << "Payment Address: " << std::endl;

	std::cout << multsigAddress.encoded() << "\n" << std::endl;

	std::cout << "Private Key Mnemonics: \n" << std::endl;
	std::cout << "Key One: " << wallet1.displayMnemonic() << std::endl;
	std::cout << "Key Two: " << wallet2.displayMnemonic() << std::endl;
	std::cout << "Key Three: " << wallet3.displayMnemonic() << "\n" <<std::endl; 



}