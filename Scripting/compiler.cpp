#include <string.h>
#include <bitcoin/bitcoin.hpp> 
#include "HD_WalletTESTNET.cpp"

using namespace bc;
using namespace bc::machine;
using namespace bc::chain;

int main()
{
	data_chunk signature;
	data_chunk pubkey; 
	decode_base16(signature, "3045022100a48e7ff587b11b262cfcd4607bc9015a857d27e54d2c74721e3d0ceca235d98f022021741a5f9f630fd825bc4eac1868df063c07d76becd80e0870a938a42f70a62801");
	decode_base16(pubkey, "021e888c1cf0e964645f1d3430647bae0b7d3817dd345377bd4d0c2379f194b1d1");
	script locking = script().to_pay_key_hash_pattern(bitcoin_short_hash(pubkey));
	script unlocking = script({operation(signature), operation(pubkey)});
	program lock(locking);
	program verify(unlocking, lock);
	if(verify.is_valid())
	{
		std::cout << "Valid" << std::endl;
	}else{
		std::cout << "invalid!" << std::endl;
	}
	code result = verify.evaluate();
	std::cout << result << std::endl;
	data_chunk op;
	for(int x=0; x < 2; x++)
	{
		op = verify.pop();
		std::cout << operation(op).to_string(0xffffffff) << std::endl;
	}





}