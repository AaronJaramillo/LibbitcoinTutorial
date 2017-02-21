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
	Channel acceptPayChannel(Channel channel)
	{
		payChannel = channel;
		payChannel.set_recieverKey(recieverKey);
		return payChannel;
	}

	Channel getChannel()
	{
		return payChannel;
	}

	void set_fillHash(hash_digest hash)
	{
		fillHash = hash;
		payChannel.setFillHash(fillHash);
	}

	endorsement signRefund()
	{
		endorsement refundSig;
		script().create_endorsement(refundSig, wallet.childPrivateKey(1).secret(), redeemScript, refund, 0, all);
		return refundSig;
	}

	void setRefund()
	{
		refund = payChannel.setRefund();
	}
	transaction getRefund()
	{
		return refund;
	}
	data_chunk public_key()
	{
		return recieverKey;
	}










private:
	HD_Wallet wallet;
	script redeemScript;
	data_chunk payerKey;
	data_chunk recieverKey;
	transaction refund;
	hash_digest fillHash;
	Channel payChannel;




};