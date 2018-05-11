#include <bitcoin/bitcoin.hpp>
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

script getWitnessScript(ec_public key1, ec_public key2, ec_public key3) {
	//make key list
	point_list keys {key1.point(), key2.point(), key3.point()};
	//create 2/3 multisig script
	script multisig = script::to_pay_multisig_pattern(2u, keys);
	return multisig;
}

script getRedeemScript(ec_public key1, ec_public key2, ec_public key3) {
	//create 2/3 multisig script
	script multisig = getWitnessScript(key1, key2, key3);

	//sha256 the script
	data_chunk multisig_hash = to_chunk(sha256_hash(multisig.to_data(0)));

	//redeem script
	operation::list redeemscript_ops {operation(opcode(0)), operation(multisig_hash)};
	script redeem_script = script(redeemscript_ops);

	return redeem_script;
}
//Generated with bx seed | bx ec-new
//bx ec-to-public [privatekey]
//private keys
// 0: e4ad55ecfd41062cec616762b73457c4d6bab35f3cb7dbe2fbab3f6a4d921bde
// 1: 3edef3e2d2152156e6816c06e65f64baff4fc0831cf25ae86df016a5cb651747
// 2: 5398432f87ad7b09d2aa6c3c2653d24a97b96fe1205d75f6d0dcf62b6c3414e1
int main() {
	//Define 3 public keys to use
	ec_public key1("02e1a13910e955222a0b5b4e1d2395f02fdb38de69c7643950ebefd0c767874448");
	ec_public key2("03c98cdcca5164394ed25fe9783c5eb289669a4cc69ff0492d3ef15b409428ce8d");
	ec_public key3("03eacfd3a4c110b88f8703bd738316dc7270a9ba95841feaa303a644d46eb8ba5a");

	//create 2/3 multisig script
	script multisig = getWitnessScript(key1, key2, key3);//script::to_pay_multisig_pattern(2u, keys);
	std::cout << "Multisig Script: " << multisig.to_string(0) << std::endl;


	//redeem script
	script redeem_script = getRedeemScript(key1, key2, key3); //script(redeemscript_ops);
	std::cout << "Redeem Script: " << redeem_script.to_string(0) << std::endl;

	//make address from script
	payment_address address = payment_address(redeem_script, payment_address::testnet_p2sh);

	std::cout << "Redeem Script Hash: " << encode_base16(address.hash()) << std::endl;
	std::cout << "Payment Address: " << address.encoded() << std::endl;

	//scriptPubKey
	script P2SH_P2WSH = script(script::to_pay_script_hash_pattern(address.hash()));

	std::cout << "Locking Script: " << P2SH_P2WSH.to_string(0) << std::endl;


}
//g++ -std=c++11 -o address P2SH-P2WSH-Address.cpp $(pkg-config --cflags libbitcoin --libs libbitcoin)


