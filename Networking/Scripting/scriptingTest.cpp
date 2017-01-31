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

	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).to_hd_key());
	data_chunk pubkey2 = to_chunk(wallet2.childPublicKey(1).to_hd_key());
	data_chunk pubkey3 = to_chunk(wallet3.childPublicKey(1).to_hd_key());

	// data_chunk m;
	// decode_base16(m, "52");
	// data_chunk n;
	// decode_base16(n, "53");
	// data_chunk rawScript = build_chunk({m, pubkey1, pubkey2, pubkey3, n, multisigOP.to_data()});
	//script multisigScript(rawScript, false);
	//operation multisigOP;
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

	// input multisigInput; 
	// multisigInput.set_sequence(42);
	// output_point previousOutput;
	// previousOutput.validation.cache.set_script(std::move(multisigScript));
	// multisigInput.set_previous_output(previousOutput);


	// transaction tx;
	// tx.set_inputs({ std::move(multisigInput) });

	// if(tx.is_valid()){
	// 	std::cout << "Transaction is Valid: \n" << std::endl;
	// } else {
	// 	std::cout << "Invalid: \n" << std::endl;
	// }


	// std::cout << encode_base16(tx.hash()) << "\n" << std::endl;


	short_hash scriptHash = ripemd160_hash(multisigScript.to_data(0));
	std::cout << encode_base16(multisigScript.to_data(0)) << "\n" <<std::endl;
	multisigScript = script(multisigScript.to_pay_script_hash_pattern(scriptHash));



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