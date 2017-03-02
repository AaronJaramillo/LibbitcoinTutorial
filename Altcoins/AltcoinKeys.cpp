#include <bitcoin/bitcoin.hpp>

#include <string.h>

using namespace bc;



void showKeys(short altcoin)
{
	data_chunk seedChunk(16);
	pseudo_random_fill(seedChunk);

	ec_secret secret = sha256_hash(seedChunk);

	wallet::ec_private privateKey(secret, altcoin);
	std::cout << "\nWIF: " << privateKey.encoded() << std::endl;
	std::cout << "Payment Address: " << privateKey.to_payment_address().encoded() <<std::endl; 
}

int main()
{
	short LTC = 0xB030; //L 0xB0, 0x30
	short DOGE = 0x9E1E; //D 0x9E, 0x1E
	short DASH = 0xCC4C; //X 0xCC, 0x4C

	showKeys(LTC);
	showKeys(DOGE);
	showKeys(DASH);

}


