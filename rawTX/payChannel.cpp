#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;

//ec_public takes base16
ec_public getPayer(std::string key){
	return ec_public(key);
}

ec_public getPayee(std::string key){
	return ec_public(key);
}

script makeRedeemScript(ec_public payer, ec_public payee){

	data_chunk p1 = to_chunk(payer.point());
	data_chunk p2 = to_chunk(payee.point());

	data_stack keys {p1, p2};
	script multisig = script(script().to_pay_multisig_pattern(2, keys));
	return multisig;

}

payment_address makeMultisigAddress(ec_public payer, ec_public payee){

	script redeem = makeRedeemScript(payer, payee);
	return payment_address(redeem, 0xc4);
}
output makeChange(uint64_t amount, uint64_t value, payment_address addy)
{
	uint64_t change = value - amount - 1000;
	script outScript = script(script().to_pay_key_hash_pattern(addy.hash()));
	
	std::cout << "change: "<< change << std::endl;
	return output(change, outScript);
}

points_value getOutputs(payment_address address, uint64_t amount){

	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet1.libbitcoin.net:19091");

	client::obelisk_client client(connection);

	points_value val1;
	static const auto on_done = [&val1](const points_value& vals) {

		std::cout << "Success: " << vals.value() << std::endl;
		val1 = vals;
		

	};

	static const auto on_error = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}

	client.blockchain_fetch_unspent_outputs(on_error, on_done, address, amount, select_outputs::algorithm::greedy);
	
	client.wait();
	
	
	//return allPoints;
	return val1;
}


transaction makeBond(payment_address multisigAddy, payment_address payerAddy, std::string amount){

	//Destination Address is the multisig addy 
	payment_address Destination = multisigAddy;
	script outputScript = script(script().to_pay_script_hash_pattern(multisigAddy.hash()));

	//Amount to fill the channel with.
	uint64_t Satoshis; 
	decode_base10(Satoshis, amount, 8);
	output output1(Satoshis, outputScript);

	///Get UTXOs
	points_value UTXOs = getOutputs(payerAddy, Satoshis);
	script previousLockingScript = script(script().to_pay_key_hash_pattern(payerAddy.hash()));

	//makes Inputs
	input::list inputs {};
	
	for (auto utxo: UTXOs.points)
	{
		input workingInput = input();
		workingInput.set_previous_output(output_point(utxo));

		std::cout << encode_base16(utxo.hash()) << std::endl;

		workingInput.set_sequence(0xffffffff);
		inputs.push_back(workingInput);
	}

	//Outputs List
	output::list outputs {output1};

	//Make Change
	if(UTXOs.value() > Satoshis)
	{
		outputs.push_back(makeChange(Satoshis, UTXOs.value(), payerAddy));
	}

	//Make Transaction 

	transaction tx = transaction();
	tx.set_inputs(inputs);
	tx.set_outputs(outputs);
	//tx.set_locktime(1490493600);
	return tx;


}

transaction makeRefund(payment_address payerAddy, payment_address multisigAddy, script redeemScript, std::string amount, transaction bond)
{
	payment_address Destination = payerAddy;
	script outputScript = script(script().to_pay_key_hash_pattern(payerAddy.hash()));

	//Amount in the channel to refund
	uint64_t Satoshis; 
	decode_base10(Satoshis, amount, 8);
	output output1((Satoshis-2000), outputScript);

	//Get UTXOs
	//points_value UTXOs = getOutputs(payerAddy, Satoshis);
	hash_digest bondHash;
	decode_hash(bondHash, "aad2ef50d74bd70892c023e81e531f061b0addbe0e4b5c5dc4354bb2c709c038");
	output_point utxo(bondHash, 0);

	//makes Inputs
	input input1 = input();
	input1.set_previous_output(utxo);
	input1.set_sequence(0xffffffff);
	input::list inputs {input1};

	//Outputs List
	output::list outputs {output1};

	transaction tx = transaction();
	tx.set_inputs(inputs);
	tx.set_outputs(outputs);
	tx.set_locktime(1490666400);
	return tx;

}

void broadcastTX(transaction tx)
{
	client::connection_type connection = {};
	connection.retries = 3;
	connection.timeout_seconds = 8;
	connection.server = config::endpoint("tcp://testnet1.libbitcoin.net:19091");

	client::obelisk_client client(connection);

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;
	}
	
	static const auto on_done = [](const code& ec) {

		std::cout << "Success: " << ec.message() << std::endl;

	};

	static const auto on_error2 = [](const code& ec) {

		std::cout << "Error Code: " << ec.message() << std::endl;

	};

	client.transaction_pool_broadcast(on_error2, on_done, tx);
	client.wait();
}

script workerOutputScript(payment_address payee)
{
	return script(script().to_pay_key_hash_pattern(payee.hash()));
}

transaction microPay(transaction tx, std::string payment, payment_address payerAddy, payment_address payeeAddy)
{
	
	script outputScript = script(script().to_pay_key_hash_pattern(payerAddy.hash()));

	//Amount in the channel to refund
	uint64_t paymentAmount;
	decode_base10(paymentAmount, payment, 8);
	if (tx.outputs().size() > 1)
	{
		std::cout << tx.outputs()[1].value() << std::endl;
		uint64_t paymentTotal = tx.outputs()[1].value() + paymentAmount;
		tx.outputs()[1].set_value(paymentTotal);
	} else {
		//output output2 = output();

		script workerScript = workerOutputScript(payeeAddy);
		output output2(paymentAmount, workerScript);
		tx.outputs().push_back(output2);

	}
	uint64_t refundTotal = tx.outputs()[0].value() - paymentAmount;
	tx.outputs()[0].set_value(refundTotal);
	std::cout << "Refund: " << tx.outputs()[0].value() << std::endl;
	std::cout << "Payment: " << tx.outputs()[1].value() << std::endl;

	//payment output
	
	//paymentAmount = paymentAmount + tx.outputs()[1].value();
	//refund output
	// channelValue = channelValue - paymentAmount - 2000;
	// output output1(channelValue, outputScript);

	// script workerScript = workerOutputScript(payeeAddy);
	// output output2(paymentAmount, workerScript);

	// output::list outs {output1, output2};

	// tx.set_outputs(outs);

	return tx;




}

int menu()
{
	std:cout << "1. MakePayment" << std::endl;
	std::cout << "2. Pay 10,000" << std::endl;
	std::cout << "3. Broadcast." << std::endl;
	std::cout << "4. Quit." << std::endl;
	int input;
	cin >> input;
	return input;
}

int main()
{
	//ec_public key1 = getPayer("");
	//ec_public key2 = getPayee("");

	HD_Wallet wallet(split("chase pair scorpion slab pause imitate dog blouse check dignity message strong"));


	ec_public payer = wallet.childPublicKey(1).point();
	ec_public payee = wallet.childPublicKey(2).point();

	payment_address channelAddy = makeMultisigAddress(payer, payee);

	script redeem = makeRedeemScript(payer, payee);
	script previousLockingScript = script(script().to_pay_key_hash_pattern(wallet.childAddress(1).hash()));

	transaction bond = makeBond(channelAddy, wallet.childAddress(1), "2");
	endorsement sig;
	script().create_endorsement(sig, wallet.childPrivateKey(1).secret(), previousLockingScript, bond, 0, all);
	data_chunk rawKey = to_chunk(wallet.childPublicKey(1).point());
	operation::list ops {operation(sig), operation(rawKey)};

	bond.inputs()[0].set_script(script(ops));


	transaction refund = makeRefund(wallet.childAddress(1), channelAddy, redeem, "2", bond);
	endorsement payeeSig; 
	endorsement payerSig;
	script().create_endorsement(payerSig, wallet.childPrivateKey(1).secret(), redeem, refund, 0, all);
	script().create_endorsement(payeeSig, wallet.childPrivateKey(2).secret(), redeem, refund, 0, all);

	data_chunk redeemScriptData = redeem.to_data(0);
	operation::list ops2 {operation(opcode(0)), operation(payerSig), operation(payeeSig), operation(redeemScriptData)};

	if(script().is_sign_multisig_pattern(ops2))
	{
		std::cout<< "Sign Multisig pattern: " << std::endl;

	}else{
		std::cout << "Not sign Multisig pattern: " << std::endl;
		
	}

	refund.inputs()[0].set_script(script(ops2));
	refund.set_version(1);
	std::cout << encode_base16(bond.to_data()) << "\n" <<std::endl;
	std::cout << encode_base16(refund.to_data()) << std::endl;

	//broadcastTX(bond);
	operation::list channel_ops {};
	transaction channel = refund;
	int control = 0;
	while (control != 4)
	{ 
		control = menu();
		if(control == 1)
		{
			channel = microPay(channel, "0.00000100", wallet.childAddress(1), wallet.childAddress(2));
			std::cout << encode_base16(channel.to_data()) << std::endl;

			
			endorsement channel_payeeSig; 
			endorsement channel_payerSig;
			script().create_endorsement(channel_payerSig, wallet.childPrivateKey(1).secret(), redeem, channel, 0, all);
			script().create_endorsement(channel_payeeSig, wallet.childPrivateKey(2).secret(), redeem, channel, 0, all);

			operation::list temp {operation(opcode(0)), operation(channel_payerSig), operation(channel_payeeSig), operation(redeemScriptData)};
			channel_ops = temp;
			//channel.inputs()[0].set_script(script(channel_op));
			refund = channel;

		} if(control == 2)
		{
			for(int x =0; x < 1000; x++)
			{
				channel = microPay(channel, "0.00000100", wallet.childAddress(1), wallet.childAddress(2));
				std::cout << encode_base16(channel.to_data()) << std::endl;

				
				endorsement channel_payeeSig; 
				endorsement channel_payerSig;
				script().create_endorsement(channel_payerSig, wallet.childPrivateKey(1).secret(), script(script().to_pay_script_hash_pattern(channelAddy.hash())), channel, 0, all);
				script().create_endorsement(channel_payeeSig, wallet.childPrivateKey(2).secret(), script(script().to_pay_script_hash_pattern(channelAddy.hash())), channel, 0, all);

				operation::list temp {operation(opcode(0)), operation(channel_payerSig), operation(channel_payeeSig), operation(redeemScriptData)};
				channel_ops = temp;
				//channel.inputs()[0].set_script(script(channel_op));
				refund = channel;
			}
		} if(control == 3){
			refund.inputs()[0].set_script(script(channel_ops));
			std::cout << encode_base16(channel.to_data()) << std::endl;
			broadcastTX(refund);
		}
	}


	refund.inputs()[0].set_script(script(channel_ops));
	std::cout << encode_base16(channel.to_data()) << std::endl;



 




}











