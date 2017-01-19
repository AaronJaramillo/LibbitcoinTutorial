#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>
#include <sstream>
using namespace bc;

class HD_Wallet
{
public:

	//constructor
	HD_Wallet(const data_chunk seed)
	{
		mnemonic = wallet::create_mnemonic(seed);
		privateKey = wallet::hd_private(seed);
		publicKey = privateKey.to_public();
	}

	HD_Wallet(const wallet::word_list mnemonicSeed)
	{
		seed = to_chunk(wallet::decode_mnemonic(mnemonicSeed));

		mnemonic = mnemonicSeed;
		privateKey = wallet::hd_private(seed);
		publicKey = privateKey.to_public();
	}

	//display output
	void displayPrivateKey()
	{
		std::cout << privateKey.encoded() << "\n" << std::endl;
	}

	void displayMnemonic()
	{
		if(wallet::validate_mnemonic(mnemonic))
		{
			for (auto i = mnemonic.begin(); i != mnemonic.end(); ++i)
	    		std::cout << *i << ' ';
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
		return wallet::ec_public(childPublicKey(index).point()).to_payment_address();
	}

private:
	//members
	data_chunk seed;
	wallet::word_list mnemonic;
	wallet::hd_private privateKey;
	wallet::hd_public publicKey;


};
void mainMenu()
{
	std::cout << "1. Generate New Wallet" << std::endl;
	std::cout << "2. Import Wallet" << std::endl;
	std::cout << "3. display" << std::endl;
	std::cout << "6. Exit" << std::endl;
}

void displayMenu()
{
	std::cout << "1. Private Key" << std::endl;
	std::cout << "2. Mnemonic" << std::endl;
	std::cout << "3. Child Private Key" << std::endl;
	std::cout << "4. Payment Address" << std::endl;
	std::cout << "5. Exit" << std::endl;
}

HD_Wallet randomGen()
{
	data_chunk seedChunk = data_chunk(16);
	pseudo_random_fill(seedChunk);
	return HD_Wallet(seedChunk);
}
int getInput()
{
	int choice;
	std::cout << "Enter a Number: "; 
	std::cin >> choice;
	return choice;
}
wallet::word_list getWordList()
{
	std::string mnemonicSeed;
	std::cout << "Enter Seed String:";
	cin.ignore();
	std::getline(cin, mnemonicSeed);
	return split(mnemonicSeed);
}
int main(){
	HD_Wallet wallet = randomGen();
	int choice = 0;
	while(choice != 6)
	{
		mainMenu();
		choice = getInput();
		if(choice == 1)
		{	
			wallet = randomGen();
		} else if(choice == 2)
		{	
			wallet::word_list mnemonicSeedList = getWordList();
			if(wallet::validate_mnemonic(mnemonicSeedList))
			{
				wallet = HD_Wallet(mnemonicSeedList);
			} else {
				std::cout << "Mnemonic Invalid!" << std::endl;
			}

		} else if (choice == 3)
		{	
			while(choice != 5){
				displayMenu();
				choice = getInput();
				std::cin.ignore();
				int index;
				
				if(choice == 1)
				{
					wallet.displayPrivateKey();
				} else if(choice == 2) 
				{
					wallet.displayMnemonic();
					std::cout << "\n" << std::endl;

				} else if (choice == 3)
				{
					index = getInput();
					wallet.displayChildPrivateKey(index);

				} else if (choice == 4)
				{
					index = getInput();
					wallet.displayAddress(index);
				}
			}
		}
	}
}