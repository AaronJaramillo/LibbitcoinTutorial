
#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>
#include <sstream>
using namespace bc;

class HD_Wallet
{
public:

	//constructor
	HD_Wallet()
	{
		entropy = data_chunk(16);
		pseudo_random_fill(entropy);
		mnemonic = wallet::create_mnemonic(entropy);
		seed = to_chunk(wallet::decode_mnemonic(mnemonic));
		privateKey = wallet::hd_private(seed, wallet::hd_private::testnet);
		publicKey = privateKey.to_public();
	}

	HD_Wallet(const data_chunk Userentropy)
	{
		entropy = Userentropy;
		mnemonic = wallet::create_mnemonic(entropy);
		seed = to_chunk(wallet::decode_mnemonic(mnemonic));
		privateKey = wallet::hd_private(seed, wallet::hd_private::testnet);
		publicKey = privateKey.to_public();
	}

	HD_Wallet(const wallet::word_list mnemonicSeed)
	{
		seed = to_chunk(wallet::decode_mnemonic(mnemonicSeed));
		//seed = to_chunk(hashSeed);
		mnemonic = mnemonicSeed;
		privateKey = wallet::hd_private(seed, wallet::hd_private::testnet);
		publicKey = privateKey.to_public();
	}

	//display output
	void dumpKeys()
	{
		displayMnemonic();
		displayPrivateKey();
		displayChildPrivateKey(1);
		displayAddress(1);

	}
	void displayPrivateKey()
	{
		std::cout << "\nPrivate Key:" << privateKey.encoded() << std::endl;
	}

	void displayMnemonic()
	{
		if(wallet::validate_mnemonic(mnemonic))
		{
			std::cout << "\n" << join(mnemonic) << std::endl;

		}else{
			std::cout << "\n" << "Mnemonic Invalid! " << std::endl;
		}
	}

	void displayChildPrivateKey(int index)
	{
		std::cout << "\nChild Key: " << childPrivateKey(index).encoded() << std::endl;
	}

	void displayAddress(int index)
	{
		std::cout << "\nAddress: " << childAddress(index).encoded() << std::endl;
	}
	void addressRange(int start, int end)
	{
		while(start != end)
		{
			displayAddress(start);
			start++;
		}
	}

	//accesor
	wallet::hd_private childPrivateKey(int index)
	{
		return privateKey.derive_private(index);
	}

	wallet::hd_public childPublicKey(int index)
	{
		return publicKey.derive_public(index);
	}

	wallet::payment_address childAddress(int index)
	{
		return wallet::payment_address(wallet::ec_public(childPublicKey(index).point()), 0x6f);
	}

private:
	//members
	data_chunk entropy;
	data_chunk seed;
	wallet::word_list mnemonic;
	wallet::hd_private privateKey;
	wallet::hd_public publicKey;


};