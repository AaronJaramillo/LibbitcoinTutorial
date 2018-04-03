#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>

using namespace bc;

int main()
{

	data_chunk seed(16);
	pseudo_random_fill(seed);
	ec_secret secretKey = bitcoin_hash(seed);

	std::string hexKey = encode_base16(secretKey);///"8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536";
	std::cout << "Hex secret: " << std::endl;
	std::cout << hexKey << std::endl;

	ec_secret rawprivateKey;
	decode_base16(rawprivateKey, hexKey);
	wallet::ec_private privateKey(rawprivateKey, 0x8000, false);
	std::cout << "\nPrivate Key Structure: " << std::endl;
	std::cout << privateKey.encoded() << std::endl;
	ec_secret keyTest = privateKey.secret();
	std::string keyTestString = encode_base16(keyTest);
	if(keyTestString == hexKey){
		std::cout << "Confirmed: " <<keyTestString << std::endl;
	} else {
		std::cout << "Error!" <<keyTestString << std::endl;
	}
	


	

	hexKey = "80" + hexKey;
	std::cout << "\nVersioned Key: " << std::endl;
	std::cout << hexKey << std::endl;
	
	data_chunk fullKey;
	data_chunk wifVersion;
	byte_array<37u> versionedKey;

	decode_base16(fullKey, hexKey);
	decode_base16(wifVersion, "0x80");
	build_checked_array(versionedKey, { wifVersion, fullKey});
	
	std::string practiceKey = encode_base58(versionedKey);
	std::cout<< "\nBy Hand:" << std::endl;
	std::cout<< practiceKey << std::endl;
	
	// create ec_pirvate object




}