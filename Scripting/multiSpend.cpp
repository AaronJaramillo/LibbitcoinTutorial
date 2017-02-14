#include <bitcoin/bitcoin.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;


//previous output 
output_point getUTXO()
{
	hash_digest previousTX;
	decode_hash(previousTX, "");
	return output_point(previousTX, 0u);
}
///Unused in this Utility
// payment_address makeMultiSig(data_chunk p1, data_chunk p2, data_chunk p3)
// {
// 	data_stack keys {p1, p2, p3};
// 	script multi = script(script().to_pay_multisig_pattern(2, keys));
// 	std::cout << "MultiSig Script: " << std::endl;
// 	std::cout << multi.to_string(0xffffffff) << std::endl;
// 	std::cout << "Hash of Script: " << std::endl;
// 	std::cout << encode_base16(bitcoin_short_hash(multi.to_data(0))) << std::endl;
// 	std::cout << "addresses: " << std::endl;
// 	std::cout << payment_address(bitcoin_short_hash(multi.to_data(0)), 0xc4) << std::endl;
// 	return payment_address(multi, 0xc4);
// }

//Previous Locking Script
script redeemScript(data_chunk p1, data_chunk p2, data_chunk p3)
{
	data_stack keys {p1, p2, p3};
	script multisigScript = script(script().to_pay_multisig_pattern(2, keys));
	return multisigScript;

}
///Unused in this Utility
///
// script previousLocking(data_chunk p1, data_chunk p2, data_chunk p3)
// {
// 	short_hash scriptHash = bitcoin_short_hash(redeemScript(p1, p2, p3).to_data(0));
// 	return script(script().to_pay_script_hash_pattern(scriptHash));
// }

//input
input makeInput()
{
	input input1 = input();
	output_point utxo = getUTXO();
	input1.set_previous_output(utxo);
	input1.set_sequence(0xffffffff);
	return input1;
}

//output
script makelockingScript()
{
	payment_address destination("n2ge1S4bLDvJKx8AGXrK5JHY2D5cReVytu");
	return (script().to_pay_key_hash_pattern(destination.hash()));
}
output makeOutput()
{
	uint64_t amount = 148292700;
	script lockingScript = makelockingScript();
	return output(amount, lockingScript);
}

//transaction object 
transaction makeTX()
{

	transaction tx = transaction(); 
	input input1 = makeInput();
	output output1 = makeOutput();
	tx.inputs().push_back(input1);
	tx.outputs().push_back(output1);
	return tx;
}

//signing 
endorsement signature(data_chunk p1, data_chunk p2, data_chunk p3, transaction tx, HD_Wallet wallet1)
{
	endorsement endorsed; 
	if(script().create_endorsement(endorsed, wallet1.childPrivateKey(1).secret(), redeemScript(p1, p2, p3), tx, 0, all))
	{
		return endorsed;
	} else {
		std::cout << "Signature Failed!! " << std::endl;
		return endorsed;
	}
}

script buildSigScript(endorsement endorsed1, endorsement endorsed2, data_chunk p1, data_chunk p2, data_chunk p3)
{
	script redeem = redeemScript(p1, p2, p3);

	operation::list ops = {operation(opcode(0)), operation(endorsed1), operation(endorsed2), operation(redeem.to_data(0))};

	if(script().is_sign_multisig_pattern(ops))
	{
		std::cout<< "Sign Multisig pattern: " << std::endl;

		return script(ops);
	}else{
		std::cout << "Not sign Multisig pattern: " << std::endl;
		return script(ops);
	}
}
//Display

int main()
{
	std::string Mnemonic1 = "";
	std::string Mnemonic2 = "";
	std::string Mnemonic3 = "";

	HD_Wallet wallet1(split(Mnemonic1));
	HD_Wallet wallet2(split(Mnemonic2));
	HD_Wallet wallet3(split(Mnemonic3));

	data_chunk p1 = to_chunk(wallet1.childPublicKey(1).point());
	data_chunk p2 = to_chunk(wallet2.childPublicKey(1).point());
	data_chunk p3 = to_chunk(wallet3.childPublicKey(1).point());

	transaction tx = makeTX();
	tx.set_version(1);


	std::cout << "\n Previous Locking: " << std::endl;
	std::cout << previousLocking(p1, p2, p3).to_string(0xffffffff) << std::endl;

	endorsement sig1 = signature(p1, p2, p3, tx, wallet1);
	endorsement sig2 = signature(p1, p2, p3, tx, wallet2);
	script sigScript = buildSigScript(sig1, sig2, p1, p2, p3);

	std::cout << "\n" <<tx.outputs()[0].script().to_string(0xffffffff) << "||" << tx.outputs()[0].value() << std::endl;

	tx.inputs()[0].set_script(sigScript);

	std::cout << encode_base16(tx.to_data(1)) << std::endl;

}

