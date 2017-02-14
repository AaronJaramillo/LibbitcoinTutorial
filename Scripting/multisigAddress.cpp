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

	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).point());
	data_chunk pubkey2 = to_chunk(wallet2.childPublicKey(1).point());
	data_chunk pubkey3 = to_chunk(wallet3.childPublicKey(1).point());

	data_stack keys {pubkey1, pubkey2, pubkey3};
	
	script multiSig = script(script().to_pay_multisig_pattern(2, keys));
	std::cout << multiSig.to_string(1) << std::endl;
	std::cout << payment_address(multiSig, 0xc4).encoded()<< std::endl;

}