//============================================================================
// Name        : Configurator.cpp
// Author      : JH
// Version     :
// Copyright   : Copyright 2023 Jimbo S. Harris. All rights reserved.
// Description : Send "configuration" messages to other Widgets
//============================================================================

#include <iostream>
#include <unistd.h>
using namespace std;
#include "Configurator.hpp"

void Configurator::local_setup()
{
#define DEBUG
	// Slow joiner problem
	usleep(100*1000);
    Pixel pixel;
	msgpack::sbuffer header(0);
	msgpack::sbuffer pay(0);
    // send "EOF" pixel
    pixel.x = -1.0f;
    pixel.y = -1.0f;
    pixel.type = iInvalid;
    pixel.weight = 0.0f;
    header.clear();
    msgpack::pack( header, pixel );

    sendMessage(&header, &pay);

    running = false;
}

bool Configurator::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	return false; // send an outbound message as a result of local_work()
}

void Configurator::local_shutdown()
{
	std::cout << "shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 4 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Configurator conf(argv[1], "", "", argv[2], argv[3]);

	cout << "running" << endl;
	conf.run();

	cout << "shutting down" << endl;
	return 0;
}
