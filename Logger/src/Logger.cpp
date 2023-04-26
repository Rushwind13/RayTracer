//============================================================================
// Name        : Logger.cpp
// Author      : JH
// Version     :
// Copyright   : Copyright 2023 Jimbo S. Harris. All rights reserved.
// Description : Capture all messages from a particular channel
//============================================================================

#include <csignal>
#include <iostream>
using namespace std;
#include "Logger.hpp"
#include "Lighting.hpp"

Logger Logger::instance;

void Logger::local_setup()
{
//#define DEBUG
}

// You will get here if a Shadow test registers a hit with an object between the intersection and a particular light
// So this point is shadowed by this light.
bool Logger::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );

    if( pixel.type != iInvalid )
    {
        instance.pixels.push_back(pixel);
    }
    else
    {
        running = false;
        std::cout << "finishing up" << std::endl;
    }

	return false; // send an outbound message as a result of local_work()
}

void Logger::local_shutdown()
{
    for( std::vector<Pixel>::iterator it = instance.pixels.begin(); it != instance.pixels.end(); ++it)
    {
        std::cout << "(" << it->x << "," << it->y << ") ";
        printvec( "o", it->r.origin);
        printvec( "d", it->r.direction);
        std::cout << endl;
    }
    std::cout << "got " << instance.pixels.size() << " pixels"<< std::endl;
	std::cout << "shutting down... ";
}

void Logger::registerHandler()
{
    signal(SIGINT, Logger::logHandler);
}

void Logger::signalHandler( int signum )
{
   cout << "Interrupt signal (" << signum << ") received.\n";

   for( std::vector<Pixel>::iterator it = pixels.begin(); it != pixels.end(); ++it)
   {
       std::cout << "(" << it->x << "," << it->y << ") ";
       printvec( "o", it->r.origin);
       printvec( "d", it->r.direction);
       std::cout << endl;
   }
   std::cout << "got " << pixels.size() << " pixels"<< std::endl;

   exit(signum);
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 4 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Logger log(argv[1], argv[2], argv[3], "", "");
    // register signal SIGINT and signal handler
    log.registerHandler();

	cout << "running" << endl;
	log.run();

	cout << "shutting down" << endl;
	return 0;
}
