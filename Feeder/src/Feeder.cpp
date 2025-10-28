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
// #define DEBUG
	// Slow joiner problem
	usleep(1000*1000);

    int pixel_count = 0;
    int current_y = -1;

	std::cout << name << " starting up... " << std::endl;

    std::ifstream in(inputFile);
    if (in.good()) {
        std::cout << "The file was opened successfully." << std::endl;
    } else {
        std::cout << "The file could not be opened." << std::endl;
    }
	msgpack::sbuffer header(0);
	msgpack::sbuffer pay(0);
    Pixel pixel;
    Intersection i;
    std::string line2;
    const char* env_limit = std::getenv("FEEDER_LIMIT");
    int limit = env_limit ? std::atoi(env_limit) : -1;
    for (std::string line; std::getline(in, line); )
    {
        ReadPixel(line, pixel);

        if( current_y != pixel.y )
        {
            current_y = pixel.y;
            std::cout << "(" << current_y << ")" << std::endl;
        }

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
        if( limit > 0 && pixel_count >= limit )
        {
            std::cout << "Reached FEEDER_LIMIT=" << limit << ", stopping early." << std::endl;
            break;
        }
    	usleep(4*1000);
    }
    std::cout << std::endl;

    if(wantEOF)
    {
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
    }

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
    std::cout << "starting up" << std::endl;
    // Expected arguments:
    // argv[1]=NAME argv[2]=INPUT_CHANNEL argv[3]=INPUT_SOCKET argv[4]=OUTPUT_CHANNEL argv[5]=OUTPUT_SOCKET [argv[6..]=file(s)]
    if( argc < 6 )
    {
        std::cout << "please use start.sh to provide proper CLI args" << std::endl;
        return 1;
    }

    // Remaining CLI args are the list of files, run them one-by-one
    for (int i = 6; i < argc; i++)
    {
        Feeder fd(argv[1], "", "", argv[4], argv[5]);
        fd.wantEOF = ( i == argc-1 );
        // Use the provided argument as the path (relative or absolute)
        snprintf(fd.inputFile, sizeof(fd.inputFile), "%s", argv[i]);

        std::cout << "running with file: " << fd.inputFile << std::endl;
        fd.run();
        std::cout << "ran with file: " << fd.inputFile << std::endl;
    }

    std::cout << "shutting down" << std::endl;
    return 0;
}
