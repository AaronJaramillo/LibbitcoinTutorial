#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>
#include <sstream>

using namespace bc;

int main()
{
	//Generate a Random Seed
	// uint64_t seed = pseudo_random();
	// // std::ostringstream seedStream;
	// // seedStream << seed;
	// std::string seedString = encode_base10(seed); //(seedStream.str());
	// std::cout << "\nSeed: " << std::endl;
	// std::cout << seedString << std::endl;

	data_chunk seedChunk(16);
	pseudo_random_fill(seedChunk);
	//data_chunk seedChunk(8); // = to_chunk(seedString); 
	//auto sink = make_unsafe_serializer(seedChunk.begin());
	//sink.write_8_bytes_big_endian(seed);

	std::cout << "\nHex Seed: " << std::endl;
	std::cout << encode_base16(seedChunk) << std::endl;
	std::cout << "\nMneumonic: " << std::endl;
	wallet::word_list writtenWord = wallet::create_mnemonic(seedChunk);
	//std::cout << writtenWord[0] << std::endl;
	if(wallet::validate_mnemonic(writtenWord)){
		for (auto i = writtenWord.begin(); i != writtenWord.end(); ++i)
    		std::cout << *i << ' ';
	}else{
		std::cout << "mnemonic invalid!" << std::endl;
	}


	wallet::hd_private privateKey(seedChunk);
	std::cout << "\n\nMaster Private Key: " << std::endl;
	std::cout << privateKey.encoded() << std::endl;

	wallet::hd_key keys = privateKey.to_hd_key();
	std::cout << "\nHex Master Private Key: " << std::endl;
	std::cout << encode_base16(keys) << std::endl;

	wallet::hd_public publicKey = privateKey.to_public();
	std::cout << "\nMaster Public Key: " << std::endl;
	std::cout << publicKey.encoded() << std::endl;

	wallet::hd_private childPrivateKey = privateKey.derive_private(1);
	std::cout << "\nChild Private Key: " << std::endl;
	std::cout << childPrivateKey.encoded() << std::endl;

	wallet::hd_public childPublicKey = privateKey.derive_public(1);
	std::cout << "\nChild Public Key: " << std::endl;
	std::cout << publicKey.encoded() << std::endl;

	std::cout << "\nPayment Adress: " << std::endl;
	std::cout << wallet::ec_public(childPublicKey.point()).to_payment_address().encoded() << "\n" << std::endl;


}