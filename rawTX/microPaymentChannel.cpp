#include <bitcoin/bitcoin.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

class Channel
{
public:

	//constructor
	Channel()
	{

	}
	Channel(data_chunk p1, data_chunk p2, uint64_t amount, uint32_t holdingPeriod)
	{
		recieverKey = p1;
		payerKey = p2; 
		channelValue = amount;
		locktime = holdingPeriod;
		paymentChannel = payment_Channel();	
	}
	// //Wallet1 
	// //
	// HD_Wallet getPayer()
	// {

	// 	HD_Wallet wallet = HD_Wallet();

	// 	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).point());

	// }
	// Wallet 2
	// HD_Wallet getPayee()
	// {

	// 	HD_Wallet wallet = HD_Wallet();

	// 	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).point());

	// }
	//Payment Channel (Multisig Addy)
	payment_address payment_Channel()
	{
		data_stack keys {recieverKey, payerKey};
		script multiSig = script(script().to_pay_multisig_pattern(2, keys));

		return payment_address(multiSig, 0xc4);

	}
	//Refund Coupon for the payer
	script outputP2KHScript(payment_address addy)
	{
		return script(script().to_pay_key_hash_pattern(addy.hash()));
	}
	script outputP2SHScript(short_hash scriptHash)
	{
		return script(script().to_pay_script_hash_pattern(scriptHash));
	}

	transaction channelFillUp(std::string hashString, int index)
	{
		hash_digest hash;  
		decode_hash(hash, hashString);
		output_point utxo(hash, index);
		input input1 = input();
		input1.set_previous_output(utxo);
		//input needs script
		input1.set_sequence(0xffffffff);
		output channel = output();
		channel.set_script(outputP2SHScript(paymentChannel.hash()));
		channel.set_value(channelValue);

		FillUp = transaction();
		FillUp.inputs().push_back(input1);
		FillUp.outputs().push_back(channel);
		///needs signing//

		return FillUp;
	}

	transaction refund()
	{
		output_point utxo(FillUpFinal.hash(), 0);
		input input1 = input()
		input1.set_previous_output(utxo);
		input1.set_sequence(0);
		//needs script//
		output output1 = output();
		output1.set_script(outputP2KHScript(bitcoin_short_hash(payerKey)));
		output1.set_value(channelValue);
		transaction refund = transaction();
		refund.inputs().push_back(input1);
		refund.outputs().push_back(output);
		return refund;

	}

	transaction getFillUp()
	{
		return FillUp;
	}
	transaction get_fillUpFinal()
	{
		return FillUpFinal;
	}
	void set_fillUpFinal(transaction tx)
	{
		FillUpFinal = tx;
	}


	// transaction refund(payment_address payeeAddress, uint64_t channelValue)
	// {
	// 	transaction tx;

	// 	output refund = output();
	// 	script outputScript = outputP2KHScript(payeeAddress);
	// 	refund.set_value(channelValue);
	// 	refund.set_script(outputScript);

	// }
	//micro tx for the payee 

	//transaction stream

private: 
	//members
	//HD_Wallet payerWallet;
	//HD_Wallet recieverWallet;

	data_chunk payerKey;
	data_chunk recieverKey;

	payment_address paymentChannel;

	endorsement bondSig;
	transaction FillUp;
	transaction FillUpFinal;
	transaction Bond;
	transaction refundBond;
	transaction channelState;

	uint64_t channelValue; 
	uint32_t locktime; 

	script sigScriptBond()
	{
		operation::list opList {operation(bondSig), operation(payerKey)};
		script sigScript(opList);
		return sigScript;

	}


};