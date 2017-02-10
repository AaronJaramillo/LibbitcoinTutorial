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
	decode_base16(signature, "");
	decode_base16(pubkey, "");
	script locking = script().to_pay_key_hash_pattern(bitcoin_short_hash(pubkey));
	script unlocking = script({operation(signature), operation(pubkey)});
	program lock(locking);
	program verify(unlocking, lock);
	// if(verify.is_valid())
	// {
	// 	std::cout << "Valid" << std::endl;
	// }else{
	// 	std::cout << "invalid!" << std::endl;
	// }
	// code result = verify.evaluate();

	code result = interpreter::run(locking.operations()[0],verify);

	std::cout << result << std::endl;
	data_chunk op;
	for(int x=0; x < 2; x++)
	{
		op = verify.pop();
		std::cout << operation(op).to_string(0xffffffff) << std::endl;
	}

	


}