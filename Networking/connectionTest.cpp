#include <bitcoin/bitcoin.hpp>
#include <bitcoin/client.hpp>
#include <string.h>
#include <iostream>
#include <cstdint>
//#include <bitcoin/explorer/callback_state.hpp>
//#include <bitcoin/explorer/define.hpp>
//#include <bitcoin/explorer/config/encoding.hpp>
//#include <bitcoin/explorer/display.hpp>
//#include <bitcoin/explorer/prop_tree.hpp>

using namespace bc;
//using namespace bc::chain;
//using namespace bc::config;
//using namespace bc::explorer;
//using namespace bc::explorer::config;

class stream_fixture
 : public client::stream
{
public:
    data_stack out;

    virtual int32_t refresh() override
    {
        return 0;
    }

    virtual bool read(stream& stream) override
    {

        return false;
    }

    virtual bool write(const data_stack& data) override
    {
        out = data;
        return true;
    }
};

static std::string to_string(data_slice data)
{
    return std::string(data.begin(), data.end());
}

int main()
{
	const auto& address = wallet::payment_address("17SEnU2rxxh8ugqh1Tpq6c9xHpQGvEjyLB");
	client::connection_type connection = {};
	connection.retries = 10;
	connection.timeout_seconds = 15;
	connection.server = config::endpoint("tcp://obelisk.airbitz.co:9091");

	client::obelisk_client client(connection);

	if(!client.connect(connection))
	{
		std::cout << "Fail" << std::endl;
	} else {
		std::cout << "Connection Succeeded" << std::endl;

	}
	static const uint32_t timeout_ms = 2000;
	static const uint32_t retries = 0;
	static const auto on_error = [](const code&) {

		std::cout << "Error Code" << std::endl;
	};
	static const auto on_unknown = [](const std::string& message) {
		std::cout << message << std::endl;
	};

	const auto on_reply = [](size_t blockHeight) 
	{
		data_chunk data(8);
		auto source = make_safe_deserializer(data.begin(), data.end());
		auto sink = make_unsafe_serializer(data.begin());
		sink.write_size_big_endian(blockHeight);
		const auto height = source.read_string();
		std::cout << height << std::endl;
	};
	stream_fixture capture;
	client::proxy proxy(capture, on_unknown, timeout_ms, retries);
	proxy.blockchain_fetch_last_height(on_error, on_reply);
	
	// client.wait();
	//std::cout << to_string(capture.out[0]) << std::endl;


	// handle1 on_error<code>();
	// client::height_handler 
	// code error_handler;
	// std::string balance;
	// std::ostream error(error_handler);
	// std::ostream output(balance);
	// const auto engine = encoding_engine();
 //    callback_state state(error, output, engine);

 //    auto on_done = [&state, &address](const chain::history::list& rows)
 //    {
 //        // This override summarizes the history response as balance.
 //        state.output(prop_tree(rows, address));
 //    };

 //    auto on_error = [&state](const code& error)
 //    {
 //        state.succeeded(error);
 //    };


 //    client.address_fetch_history2(on_error, on_done, address);
 //    client.wait();

 //    std::cout << state.get_result() << std::endl;






}