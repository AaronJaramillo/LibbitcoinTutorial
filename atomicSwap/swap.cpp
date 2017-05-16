#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_WalletTESTNET.cpp"
#include <string>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;



script contractScript(payment_address seller, payment_address buyer, uint32_t locktime, short_hash hashLock)
{
	std::string scriptString = "if dup hash160 [" + encode_base16(buyer.hash()) + "] equalverify checksigverify hash160 [" + encode_base16(hashLock) + "] equal else [" + encode_base10(locktime) + "] checklocktimeverify drop dup hash160 [" + encode_base16(seller.hash()) + "] equalverify checksig";
	script contract = script();
	contract.from_string(scriptString);
	return contract;
}

payment_address contractAddress(payment_address seller, payment_address buyer, uint32_t locktime, short_hash hashLock, uint8_t chainCode)
{
	script contract = contractScript(seller, buyer, locktime, hashLock);
	std::cout << encode_base16(contract.to_data(0)) << std::endl;
	return payment_address(contract, chainCode);
}
uint32_t setLocktime(uint32_t days)
{
	uint32_t seconds = days*86400;
	uint32_t now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	uint32_t timeLock = now + seconds;
	return timeLock;
}

short_hash makeHashLock(std::string secret)
{
	data_chunk secretChunk = to_chunk(secret);
	short_hash hashLock = bitcoin_short_hash(secretChunk);
	return hashLock;
}
transaction sigScript(ec_private wal, transaction tx)
{
	int index = 0;
	for (auto input: tx.inputs())
	{

		endorsement sig;
		script().create_endorsement(sig, wal.secret(), input.script(), tx, index, all);
		operation::list ops {operation(sig), operation(to_chunk(wal.to_public().point()))};
		script scriptSig(ops);
		input.script().clear();
		input.set_script(scriptSig);
		tx.inputs()[index] = input;
		index++;
	}
	return tx;

}

transaction withdrawContract(transaction contractPayment, script redeemScript, uint64_t amount, data_chunk hashKey, ec_private key)
{
	transaction tx = transaction();
	// hash_digest utxoHash;
	// decode_hash(utxoHash, "");
	output_point utxo(contractPayment.hash(), 0);

	input input1 = input();
	input1.set_previous_output(utxo);
	tx.inputs().push_back(input1);

	script outputScript(script().to_pay_key_hash_pattern(key.to_payment_address().hash()));
	output output1(amount, outputScript);
	tx.outputs().push_back(output1);

	endorsement sig; 
	script().create_endorsement(sig, key.secret(), redeemScript, tx, 0, all);
	operation::list resolveHTLC {operation(hashKey), operation(sig), operation(to_chunk(key.to_public().point())), operation(opcode(81)), operation(redeemScript.to_data(0))};

	tx.inputs()[0].set_script(script(resolveHTLC));

	return tx;
}
transaction payToContract(payment_address contract, uint64_t amount, ec_private key)
{
	transaction tx = transaction();
	hash_digest utxoHash; 
	decode_hash(utxoHash, "0e600c5a7ee3055dee38b08379a020fa5eb013bdffe8742fb9f0a802e7011d50");
	output_point utxo(utxoHash, 0);
	input input1 = input();
	input1.set_previous_output(utxo);
	input1.set_script(script(script().to_pay_key_hash_pattern(key.to_payment_address())));
	tx.inputs().push_back(input1);

	script outputScript(script().to_pay_script_hash_pattern(contract.hash()));
	output output1(amount, outputScript);
	tx.outputs().push_back(output1);

	return sigScript(key, tx);

}


int main(){
	payment_address aliceBTC_address("mpEwnuvRJxRC7R4KxjdooMFRUNRYaQxwWD");
	payment_address bobBTC_address("moRFyt8S8YTCJkgHktoqVZ73zWb8crevh2");
	script BTCcontract = contractScript(aliceBTC_address, bobBTC_address, setLocktime(14), makeHashLock("Libbitcoin"));
	payment_address BTCcontract_address = payment_address(BTCcontract, 196);
	ec_private alicBTC_private("cR3FqqoLz6b5wmA5h7LHzTJYosbcuZGsrQ2Fse8V2q2jQtesarmg", 239);
	ec_private bobLTC_private("cSASaBNPZb2aRFUkK5VADEvTZJpTVFjpV4UvP7dnJETM9ycFX3f2", 239);

	std::cout << "BTC contract Address: " << BTCcontract_address.encoded() << std::endl;
	std::cout << "HashLock: " << encode_base16(makeHashLock("Libbitcoin")) << "\n" <<std::endl;
	transaction BTCpayment = payToContract(BTCcontract_address, 100000000, alicBTC_private);
	std::cout << encode_base16(BTCpayment.to_data(1)) << "\n" << std::endl;
	short_hash hashLock;
	decode_base16(hashLock, "7e88c8277e78610110c79a77eb0d340fba0c2775");
	script LTCcontract = contractScript(bobBTC_address, aliceBTC_address, setLocktime(7), hashLock);
	payment_address LTCcontract_address = payment_address(LTCcontract, 196);
	std::cout << "LTC contract Address: " << LTCcontract_address.encoded() << "\n"<<std::endl;
	transaction LTCpayment = payToContract(LTCcontract_address, 1000000000, bobLTC_private);
	std::cout << encode_base16(LTCpayment.to_data(1)) << "\n" <<std::endl;

	data_chunk hashKey;
	std::cout << encode_base16(withdrawContract(LTCpayment, LTCcontract, 1000000000, to_chunk("Libbitcoin"), alicBTC_private).to_data(1)) << "\n" << std::endl;

	std::cout << encode_base16(withdrawContract(BTCpayment, BTCcontract, 100000000, to_chunk("Libbitcoin"), bobLTC_private).to_data(1)) << std::endl;

}