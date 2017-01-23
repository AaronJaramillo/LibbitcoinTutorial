#include <bitcoin/bitcoin.hpp>
#include <string.h>
#include <iostream>
#include "HD_Wallet.cpp"

using namespace bc;

void mainMenu()
{
	std::cout << "\n==========================" << std::endl;
	std::cout << "1. Generate New Wallet" << std::endl;
	std::cout << "2. Import Wallet" << std::endl;
	std::cout << "3. display" << std::endl;
	std::cout << "4. Exit" << std::endl;
	std::cout << "==========================" << std::endl;
}

void displayMenu()
{
	std::cout << "\n==========================" << std::endl;
	std::cout << "1. Private Key" << std::endl;
	std::cout << "2. Mnemonic" << std::endl;
	std::cout << "3. Child Private Key" << std::endl;
	std::cout << "4. Payment Address" << std::endl;
	std::cout << "5. Address Range" << std::endl; 
	std::cout << "6. Dump Keys" << std::endl;
	std::cout << "7. Exit" << std::endl;
	std::cout << "==========================" << std::endl;
}

HD_Wallet randomGen()
{
	data_chunk entropyChunk = data_chunk(16);
	pseudo_random_fill(entropyChunk);
	HD_Wallet(entropyChunk).dumpKeys();
	return HD_Wallet(entropyChunk);
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
	while(choice != 4)
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
				wallet.dumpKeys();
			} else {
				std::cout << "Mnemonic Invalid!" << std::endl;
			}

		} else if (choice == 3)
		{	
			while(choice != 7){
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

				} else if (choice == 3)
				{
					index = getInput();
					wallet.displayChildPrivateKey(index);

				} else if (choice == 4)
				{
					index = getInput();
					wallet.displayAddress(index);
				} else if (choice == 5)
				{
					int start = getInput();
					int end = getInput();
					wallet.addressRange(start, end);
				}else if (choice == 6)
				{
					wallet.dumpKeys();
				}else if (choice == 7)
				{
					std::cout << "Bye" << std::endl;
				} else
				{
					std::cout << "Selection Invalid! " << std::endl;
				}
			}
		} else if (choice == 4)
		{
			std::cout << "Goodbye" << std::endl;
		}else {
			std::cout << "Selection Invalid!" << std::endl;
		}
	}
}