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

void Feeder::local_setup()
{
	std::cout << name << " starting up... ";
    // open file
    std::ifstream in(inputFile);
    Pixel pixel;
    for (std::string line; std::getline(in, line); )
    {
        ReadPixel(line, pixel);
        PrintPixel(cout, pixel);
    	usleep(1*1000);
    }

    std::cout << "all done." << std::endl;
    // file says how big it is?
    // send items from the file, line by line
    running = false;
}

bool Feeder::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ")";
	printvec("c", pixel.color);
#endif /* DEBUG */

    if( pixel.type == iInvalid )
    {
        running = false;
        std::cout << "finishing up" << std::endl;
        return false;
    }

#ifdef DEBUG
	std::cout << std::endl;
#endif /* DEBUG */
	return false; // no more messages; we're done.
}


void Feeder::local_shutdown()
{
	std::cout << "Feeder shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 5 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Feeder fd(argv[1], argv[2], argv[3], "", "");
    strcpy(fd.inputFile, argv[4]);

	cout << "running" << endl;
	fd.run();

	cout << "shutting down" << endl;
	return 0;
}
