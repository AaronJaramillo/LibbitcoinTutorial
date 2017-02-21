#include <bitcoin/bitcoin.hpp>

#include <string.h>
// #include "microPaymentChannel.cpp"


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

class Payer
{
public:

	//constructor
	Payer(std::string mnemonic)
	{
		wallet = HD_Wallet(split(mnemonic));
		payerKey = to_chunk(wallet.childPublicKey(1).point());
	}
	// void setReciever(data_chunk payee)
	// {
	// 	recieverKey = payee;
	// }
	// data_chunk getRecieverKey()
	// {
		
	// 	return recieverKey;
	// }

	Channel requestChannel(uint64_t amount, uint32_t holdingPeriod)
	{
		//recieverKey = getRecieverKey();
		payChannel = Channel(payerKey, amount, holdingPeriod);
		return payChannel;
	}
	void confirmChannel(Channel completeChannel)
	{
		payChannel = completeChannel;
	}
	Channel getChannel()
	{
		return payChannel;
	}

	void makefillUp()
	{
		fillUp = payChannel.channelFillUp("599cc7320426d23908713e58040984a98f83b7c18759765695f938792835ded6",0);
		endorsement sig = signfillUp();
		script sigScript = fillUpSigScript(sig);
		fillUp.inputs()[0].set_script(sigScript);

	}

	transaction getfillUp()
	{
		return fillUp;
	}

	script fillUpSigScript(endorsement signature)
	{
		operation::list opList {operation(signature), operation(payerKey)};
		return script(opList);
	}

	endorsement signfillUp()
	{
		script prevlockingScript = script(script().to_pay_key_hash_pattern(wallet.childAddress(1).hash()));
		endorsement signature;
		if(script().create_endorsement(signature, wallet.childPrivateKey(1).secret(), prevlockingScript, fillUp, 0, all))
		{
			//std::cout << "Sig: " << encode_base16(signature) << std::endl;
			return signature;
		} else {
			return signature;
		}
	}
	void setRefundSig(endorsement sig)
	{
		refundSig = sig;
	}

	transaction setState(int extraAmount)
	{
		state = transaction();	
		hash_digest hash = fillUp.hash(all);

		output_point utxo(hash, 0);
		
		input input1 = input();
		input1.set_previous_output(utxo);
		input1.set_sequence(0);

		state.inputs().push_back(input1);
		//needs script//
		output output1 = output();
		output1.set_script(outputP2KHScript(payment_address(ec_public(recieverKey), 0x6f)));
		output1.set_value((payChannel.getCurrentValue() + extraAmount));
		state.outputs().push_back(output1);
		output2.set_script(outputP2KHScript(payment_address(ec_public(payerKey), 0x6f)));
		output2.set_value(payChannel.getChannelValue() - (payChannel.getCurrentValue() + extraAmount + 1000));
		state.outputs().push_back(output2);

		state.set_locktime(1487552300);
		std::cout << encode_base16(state.to_data(1)) << std::endl;
		
		return state;

	}

	void payStream()
	{
		int x = 1;
		while(x == 1)
		{
			setState(10000);
			cin >> x;

		}

	}

	// void getRefundSig()
	// {


	// }

private:
	HD_Wallet wallet;
	data_chunk payerKey;
	data_chunk recieverKey;
	transaction fillUp; 
	transaction Bond;
	transaction state; 
	Channel payChannel;
	endorsement refundSig;



};