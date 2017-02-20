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
	// void getRefundSig()
	// {


	// }

private:
	HD_Wallet wallet;
	data_chunk payerKey;
	data_chunk recieverKey;
	transaction fillUp; 
	transaction Bond; 
	Channel payChannel;
	endorsement refundSig;



};