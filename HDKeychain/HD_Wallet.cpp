
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
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
		privateKey = wallet::hd_private(seed);
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
			//return join(mnemonic);
			std::string mnemonicString = join(mnemonic);
			std::cout << "\n" << mnemonicString << std::endl;
			// for (auto i = mnemonic.begin(); i != mnemonic.end(); ++i)
	  //   		std::cout << *i << ' ';
		}else{
			std::cout << "mnemonic invalid!" << std::endl;
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

	void getBalance(int index)
	{
		
		client::connection_type connection = {};
		connection.retries = 3;
		connection.timeout_seconds = 8;
		connection.server = config::endpoint("tcp://testnet.libbitcoin.net:19091");

		client::obelisk_client client(connection);


		static const auto on_done = [](const chain::history::list rows)
		{
			
			std::cout<< encode_base10(rows[0].value, 8) <<std::endl;

		};
		static const auto on_error2 = [](const code ec) {

			std::cout << "Error Code: " << ec.message() << std::endl;

		};


		if(!client.connect(connection))
		{
			std::cout << "Fail" << std::endl;
		} else {
			std::cout << "Connection Succeeded" << std::endl;
		}

		client.blockchain_fetch_history2(on_error2, on_done, childAddress(index));
		client.wait();


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
		return wallet::ec_public(childPublicKey(index).point(), TESTNET).to_payment_address();
	}

private:
	//members
	data_chunk entropy;
	data_chunk seed;
	wallet::word_list mnemonic;
	wallet::hd_private privateKey;
	wallet::hd_public publicKey;


};