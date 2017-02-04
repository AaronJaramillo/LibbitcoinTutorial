import sys
import zmq.asyncio
import asyncio

loop = zmq.asyncio.ZMQEventLoop()
asyncio.set_event_loop(loop)

import libbitcoin
context = libbitcoin.Context()

async def main():
    # The settings parameter is optional.
    # Use help(client_settings) to see more options.
    client_settings = libbitcoin.ClientSettings()
    client_settings.query_expire_time = None

    client = context.Client("tcp://obelisk-testnet.airbitz.co:9091",
                            settings=client_settings)

    ec, height = await client.last_height()
    if ec:
        print("Couldn't fetch last_height:", ec, file=sys.stderr)
        context.stop_all()
        return
    print("Last height:", height)

    context.stop_all()

if __name__ == '__main__':
    tasks = [
        main(),
    ]
    tasks.extend(context.tasks())
    loop.run_until_complete(asyncio.wait(tasks))
    loop.close()