#include <iostream>
#include <memory>
#include <bitcoin/client.hpp>


using namespace bc;
using namespace bc::client;
using namespace bc::protocol;

/**
 * A minimal example that connects to a server and fetches height.
 */
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <server>" << std::endl;
        return 1;
    }

    // Set up the server connection.
    zmq::context context;
    zmq::socket socket(context, zmq::socket::role::dealer);

    if (socket.connect({ argv[1] }) != error::success)
    {
        std::cerr << "Cannot connect to " << argv[1] << std::endl;
        return 1;
    }

    const auto unknown_handler = [](const std::string& command)
    {
        std::cout << "unknown command: " << command << std::endl;
    };

    const auto error_handler = [](const code& code)
    {
        std::cout << "error: " << code.message() << std::endl;
    };

    const auto completion_handler = [](size_t height)
    {
        std::cout << "height: " << height << std::endl;
    };

    socket_stream stream(socket);

    // Wait 2 seconds for the connection, with no failure retries.
    proxy proxy(stream, unknown_handler, 2000, 0);

    // Make the request.
    proxy.blockchain_fetch_last_height(error_handler, completion_handler);

    zmq::poller poller;
    poller.add(socket);

    // Wait 1 second for the response.
    if (poller.wait(1000).contains(socket.id()))
        stream.read(proxy);

    return 0;
}





// #include <bitcoin/bitcoin.hpp>
// #include <bitcoin/client.hpp>
// #include <string.h>
// #include <iostream>
// #include <cstdint>
// //#include <bitcoin/explorer/callback_state.hpp>
// //#include <bitcoin/explorer/define.hpp>
// //#include <bitcoin/explorer/config/encoding.hpp>
// //#include <bitcoin/explorer/display.hpp>
// //#include <bitcoin/explorer/prop_tree.hpp>

// using namespace bc;
// //using namespace bc::chain;
// //using namespace bc::config;
// //using namespace bc::explorer;
// //using namespace bc::explorer::config;

// class stream_fixture
//  : public client::stream
// {
// public:
//     data_stack out;

//     virtual int32_t refresh() override
//     {
//         return 0;
//     }

//     virtual bool read(stream& stream) override
//     {

//         return false;
//     }

//     virtual bool write(const data_stack& data) override
//     {
//         out = data;
//         return true;
//     }
// };

// // static std::string to_string(data_slice data)
// // {
// //     return std::string(data.begin(), data.end());
// // }

// int main()
// {
// 	const auto& address = wallet::payment_address("17SEnU2rxxh8ugqh1Tpq6c9xHpQGvEjyLB");
// 	client::connection_type connection = {};
// 	connection.retries = 10;
// 	connection.timeout_seconds = 15;
// 	connection.server = config::endpoint("tcp://libbitcoin1.thecodefactory.org:30000");



// 	static const uint32_t timeout_ms = 3000;
// 	static const uint32_t retries = 3;

// 	const auto on_reply = [](size_t blockHeight) 
// 	{
// 		std::cout << "height: " << blockHeight << std::endl;
// 		// data_chunk data(8);		
// 		// auto source = make_safe_deserializer(data.begin(), data.end());
// 		// auto sink = make_unsafe_serializer(data.begin());
// 		// sink.write_size_big_endian(blockHeight);
// 		// const auto height = source.read_string();
// 		// std::cout << height << std::endl;
// 	};

// 	static const auto on_error = [](const code& ec) {

// 		std::cout << "Error Code: " << ec.message() << std::endl;

// 	};
// 	static const auto on_unknown = [](const std::string& message) {
// 		std::cout << message << std::endl;
// 	};


// 	stream_fixture capture;
// 	client::obelisk_client client(connection);

// 	if(!client.connect(connection))
// 	{
// 		std::cout << "Fail" << std::endl;
// 	} else {
// 		std::cout << "Connection Succeeded" << std::endl;


// 	}
// 	//client.connect(connection);
// 	client.blockchain_fetch_last_height(on_error, on_reply);
// 	client.wait();
// 	//client::proxy proxy(capture, on_unknown, timeout_ms, retries);
	
// 	//proxy.wait()
// 	//const auto height = source.read_string();
// 	//std::cout << height << std::endl;
// 	// client.wait();
// 	//std::cout << to_string(capture.out[0]) << std::endl;


// 	// handle1 on_error<code>();
// 	// client::height_handler 
// 	// code error_handler;
// 	// std::string balance;
// 	// std::ostream error(error_handler);
// 	// std::ostream output(balance);
// 	// const auto engine = encoding_engine();
//  //    callback_state state(error, output, engine);

//  //    auto on_done = [&state, &address](const chain::history::list& rows)
//  //    {
//  //        // This override summarizes the history response as balance.
//  //        state.output(prop_tree(rows, address));
//  //    };

//  //    auto on_error = [&state](const code& error)
//  //    {
//  //        state.succeeded(error);
//  //    };


//  //    client.address_fetch_history2(on_error, on_done, address);
//  //    client.wait();

//  //    std::cout << state.get_result() << std::endl;






// }