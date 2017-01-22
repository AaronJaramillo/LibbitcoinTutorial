#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>
#include <sstream>

using namespace bc;

int main()
{
	data_chunk seed(64);
	pseudo_random_fill(seed);
	std::cout << "Seed: "<< encode_base16(wallet::hd_private(seed).to_hd_key()) << std::endl;
	wallet::word_list wordSeed = wallet::create_mnemonic(seed);
	std::string stringWord = join(wordSeed);
	std::cout << "Words: " <<  stringWord << std::endl;
	
	const auto rewordSeed = split(stringWord);
	const auto reSeed = wallet::decode_mnemonic(rewordSeed);
	data_chunk seedChunk = to_chunk(reSeed);
	std::cout << "Seed: "<< encode_base16(seedChunk) << std::endl;//encode_base16(seedChunk) 
	std::cout << "NewNew: " << join(wallet::create_mnemonic(seedChunk));
	if(seedChunk == seed)
	{
		std::cout << "YES" << std::endl;
	}else {
		std::cout << "No" << std::endl;
	}


}