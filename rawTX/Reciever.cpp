#include <bitcoin/bitcoin.hpp>

#include <string.h>
// #include "microPaymentChannel.cpp"


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;


class Reciever
{
public:

	Reciever(std::string mnemonic)
	{
		wallet = HD_Wallet(split(mnemonic));
		recieverKey = to_chunk(wallet.childPublicKey(1).point());
	}

	//create refund
	setPaychannel(Channel channel)
	{
		payChannel = channel;
	}

	endorsement signRefund()
	{
		endorsement refundSig;
		script().create_endorsement(refundSig, wallet.childPrivateKey(1).secret(), redeemScript, refund, 0, all);
		return refundSig;
	}











private:
	HD_Wallet wallet;
	data_chunk payerKey;
	data_chunk recieverKey;
	transaction refund;
	Channel payChannel;




}