#include <bitcoin/bitcoin.hpp>
#include "microPaymentChannel.cpp"
#include "Payer.cpp"
#include "Reciever.cpp"
//#include "HD_walletTESTNET.cpp"
#include <string.h>


using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;


bool test_payment_channel(Channel paymentChannel)
{
	payment_address addy = paymentChannel.payment_Channel();
	std::cout << "Payment Channel Test Output: " << std::endl;
	std::cout << addy.encoded() << std::endl;
	if(addy.hash()[-1] == 0xc4)
	{
		return 1;
	} else{
		return 0;
	}
}
bool test_channel_fill_up(Payer channelPayer)
{
	channelPayer.requestChannel(148192700, 20);
	channelPayer.makefillUp();
	transaction tx = channelPayer.getfillUp();
	std::cout << "Channel Fill Test: " << std::endl;
	std::cout << encode_base16(tx.to_data(1)) << std::endl;
	if(tx.is_valid()){
		return 1;
	} else {
		return 0;
	}
}
bool test_sign_refund(Reciever channelReciever)
{
	channelReciever.getRefund();
	endorsement refundSig1 = channelReciever.signRefund();
	std::cout << encode_base16(refundSig1) << std::endl;
	return 1;

}
// bool test_valid_bond(Channel paymentChannel, HD_Wallet wallet)
// {
// 	//paymentChannel.channelFillUp("599cc7320426d23908713e58040984a98f83b7c18759765695f938792835ded6", 0);
// 	endorsement sig = signBond(paymentChannel, wallet);
// 	paymentChannel.set_bondSig(sig);
// 	paymentChannel.channelFillUp("599cc7320426d23908713e58040984a98f83b7c18759765695f938792835ded6", 0);
// 	std::cout << "Valid Bond Test: " << std::endl;
// 	paymentChannel.validBond();
// 	std::cout << encode_base16(paymentChannel.getBond().to_data(0)) << std::endl;
// 	if(paymentChannel.validBond().is_valid()){
// 		return 1;
// 	} else {
// 		return 0;
// 	}


// }
void test_runner(bool code)
{
	if(code)
	{
		std::cout << "Test: 1" << std::endl;
	} else {
		std::cout << "Test: 0" << std::endl;

	}
}


int main()
{
	HD_Wallet wallet1 = HD_Wallet(split("chase pair scorpion slab pause imitate dog blouse check dignity message strong"));
	HD_Wallet wallet2 = HD_Wallet(split("logic waste merit drama fatal pen type embody room ladder skin chicken"));


	data_chunk p1 = to_chunk(wallet1.childPublicKey(1).point());
	data_chunk p2 = to_chunk(wallet2.childPublicKey(1).point());
	Channel paymentChannel(p2, p1, 100000000, 6);

	test_runner(test_payment_channel(paymentChannel));

	Payer channelPayer("chase pair scorpion slab pause imitate dog blouse check dignity message strong");
	Reciever channelReciever("logic waste merit drama fatal pen type embody room ladder skin chicken");
	

	test_runner(test_channel_fill_up(channelPayer));
	//paymentChannel.channelFillUp("599cc7320426d23908713e58040984a98f83b7c18759765695f938792835ded6", 0);


	channelPayer.requestChannel(148192700, 20);
	channelPayer.makefillUp();
	transaction tx = channelPayer.getfillUp();
	channelReciever.setPayChannel(channelPayer.getChannel());
	test_runner(test_sign_refund(channelReciever));
	return 0; 
}
