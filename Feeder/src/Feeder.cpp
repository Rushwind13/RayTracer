//============================================================================
// Name        : Feeder.cpp
// Author      :
// Version     :
// Copyright   : Copyright 2023 Jimbo S. Harris. All rights reserved.
// Description :
//============================================================================

#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;
#include "Feeder.hpp"
#include "Pixel.hpp"
#include "Intersection.hpp"

void Feeder::local_setup()
{
#define DEBUG
	// Slow joiner problem
	usleep(1000*1000);

    int pixel_count = 0;

	std::cout << name << " starting up... " << std::endl;

    std::ifstream in(inputFile);
	msgpack::sbuffer header(0);
	msgpack::sbuffer pay(0);
    Pixel pixel;
    Intersection i;
    std::string line2;
    for (std::string line; std::getline(in, line); )
    {
        ReadPixel(line, pixel);

        std::cout << "(" << pixel.y << ")" << "\r";

        std::getline(in, line2);
        ReadIntersection(line2, i);
#ifdef DEBUG
        // PrintPixel(cout, pixel);
        // PrintIntersection(cout, i);
#endif /* DEBUG */

        header.clear();
        msgpack::pack( &header, pixel );

        pay.clear();
        msgpack::pack( &pay, i );

        sendMessage(&header, &pay);
        pixel_count++;
    	usleep(4*1000);
    }
    std::cout << std::endl;

#define WANT_EOF
#ifdef WANT_EOF
    // send "EOF" pixel
    std::cout << "sending EOF after " << pixel_count << " pixels...";
    NewPixel(pixel);
    pixel.x = -1.0f;
    pixel.y = -1.0f;
    pixel.type = iInvalid;
    pixel.weight = 0.0f;
	header.clear();
    pay.clear();
	msgpack::pack( header, pixel );

	sendMessage(&header, &pay);
#endif /* WANT_EOF */

    std::cout << "all done." << std::endl;
    pixel_count = 0;
    running = false;
}

bool Feeder::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	return false; // all work happens in local_setup()
}


void Feeder::local_shutdown()
{
	std::cout << "Feeder shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 7 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Feeder fd(argv[1], argv[2], argv[3], argv[4], argv[5]);
    strcpy(fd.inputFile, argv[6]);

	cout << "running" << endl;
	fd.run();

	cout << "shutting down" << endl;
	return 0;
}
