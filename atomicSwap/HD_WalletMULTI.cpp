#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
using namespace bc;
//private //public //P2KH //P2SH  


class HD_Wallet
{
public:

	static const std::vector<uint32_t> LTCtestnet {0x04358394, 0x04358394, 111, 196};
	static const std::vector<uint32_t> LTC = {0x0488ADE4, 0x0488B21E, 48, 50};
	static const std::vector<uint32_t> BTCtestnet = {static_cast<unsigned int>(wallet::hd_private::testnet), static_cast<unsigned int>(wallet::hd_public::testnet), 0x6f, 0xC4};
	static const std::vector<uint32_t> BTC = {static_cast<unsigned int>(wallet::hd_private::mainnet), static_cast<unsigned int>(wallet::hd_public::mainnet), 0x00, 5};
	
	//constructor
	HD_Wallet(std::vector<uint32_t> version=BTC)
	{
		
		setChain(version);
		entropy = data_chunk(16);
		pseudo_random_fill(entropy);
		mnemonic = wallet::create_mnemonic(entropy);
		seed = to_chunk(wallet::decode_mnemonic(mnemonic));
		privateKey = wallet::hd_private(seed, wallet::hd_private::to_prefixes(prefix_HDpriv, prefix_HDpub));
		publicKey = privateKey.to_public();
	}

	HD_Wallet(const data_chunk Userentropy)
	{
		entropy = Userentropy;
		mnemonic = wallet::create_mnemonic(entropy);
		seed = to_chunk(wallet::decode_mnemonic(mnemonic));
		privateKey = wallet::hd_private(seed, prefix_HDpriv);
		publicKey = privateKey.to_public();
	}

	HD_Wallet(const wallet::word_list mnemonicSeed)
	{
		seed = to_chunk(wallet::decode_mnemonic(mnemonicSeed));
		//seed = to_chunk(hashSeed);
		mnemonic = mnemonicSeed;
		privateKey = wallet::hd_private(seed, prefix_HDpriv);
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

	void getBalance(int index)
	{
		
		client::connection_type connection = {};
		connection.retries = 3;
		connection.timeout_seconds = 8;
		connection.server = config::endpoint("tcp://testnet.libbitcoin.net:19091");

		client::obelisk_client client(connection);


		static const auto on_done = [](const chain::history::list& rows)
		{
			uint64_t unspent_balance = 0;

			for(const auto& row: rows)
			{

			    // spend unconfirmed (or no spend attempted)
			    if (row.spend.hash() == null_hash)
			        unspent_balance += row.value;
			}
			std::cout<< encode_base10(unspent_balance, 8) << std::endl;

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

		client.blockchain_fetch_history3(on_error2, on_done, childAddress(index));
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
		return wallet::payment_address(wallet::ec_public(childPublicKey(index).point()), prefix_P2KH);
	}

private:
	//members

	uint32_t prefix_HDpriv;
	uint32_t prefix_HDpub;
	uint32_t prefix_P2KH;
	uint32_t prefix_P2SH;
	std::vector<uint32_t> version;
	void setChain(std::vector<uint32_t> version)
	{

		uint32_t prefix_HDpriv = version[0];
		uint32_t prefix_HDpub = version[1];
		uint32_t prefix_P2KH = version[2];
		uint32_t prefix_P2SH = version[3];
	}

	data_chunk entropy;
	data_chunk seed;
	wallet::word_list mnemonic;
	wallet::hd_private privateKey;
	wallet::hd_public publicKey;


};