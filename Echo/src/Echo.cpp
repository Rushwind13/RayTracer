//============================================================================
// Name        : Echo.cpp
// Author      : JH
// Description : Pass-through widget: republishes incoming header+payload
//============================================================================

#include <iostream>
#include <unistd.h>
using namespace std;

#include "Echo.hpp"
#include "Pixel.hpp"

void Echo::local_setup()
{
    // Slow joiner mitigation
    usleep(100*1000);
}

bool Echo::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
    // Optional: detect EOF pixel to print progress, but always forward
    try {
        msgpack::object obj;
        unPackPart(header, &obj);
        Pixel px;
        obj.convert(px);
        if (px.type == iInvalid && px.x == -1 && px.y == -1) {
            cout << "[Echo] forwarding EOF" << endl;
        }
    } catch(...) {
        // Non-Pixel header; still forward
    }

    // Return true to send the same header+payload as received
    return true;
}

void Echo::local_shutdown()
{
    cout << "Echo shutting down..." << endl;
}

int main(int argc, char* argv[])
{
    cout << "starting Echo" << endl;
    // Expected: argv[1]=NAME argv[2]=INPUT_CHANNEL argv[3]=INPUT_SOCKET argv[4]=OUTPUT_CHANNEL argv[5]=OUTPUT_SOCKET
    if (argc != 6) {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }

    Echo e(argv[1], argv[2], argv[3], argv[4], argv[5]);
    // Allow binding modes via environment to serve as a bridge (Feeder connects -> Echo binds SUB; Writer binds SUB -> Echo connects PUB)
    const char* bind_sub = std::getenv("ECHO_BIND_SUB");
    if (bind_sub && *bind_sub && *bind_sub != '0') {
        e.forceBindSubscriber();
    }
    const char* bind_pub = std::getenv("ECHO_BIND_PUB");
    if (bind_pub && *bind_pub && *bind_pub != '0') {
        e.forceBindPublisher();
    }
    e.run();
    cout << "Echo done" << endl;
    return 0;
}
