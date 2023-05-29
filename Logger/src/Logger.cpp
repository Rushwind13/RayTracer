//============================================================================
// Name        : Logger.cpp
// Author      : JH
// Version     :
// Copyright   : Copyright 2023 Jimbo S. Harris. All rights reserved.
// Description : Capture all messages from a particular channel
//============================================================================

#include <csignal>
#include <iostream>
#include <unistd.h>
using namespace std;
#include "Logger.hpp"
#include "Lighting.hpp"

Logger Logger::instance;

void Logger::local_setup()
{
//#define DEBUG
	// Slow joiner problem
	usleep(100*1000);
}

bool Logger::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );

    std::cout << "(" << pixel.y << ")" << "\r";

    Intersection i;
    msgpack::object obj2;
    unPackPart( payload, &obj2 );
    obj2.convert( i );

    if( pixel.type != iInvalid )
    {
        instance.pixels.push_back(pixel);
        instance.intersections.push_back(i);
    }
    else
    {
        std::cout << "received EOF, writing file...";
        writeFile();
        std::cout << "done." << std::endl;
        running = false;
    }

    usleep(1*1000);
	return false; // never send outgoing messages
}

void Logger::local_shutdown()
{
    //writeFile();
	std::cout << "shutting down... ";
}

void Logger::registerHandler()
{
    signal(SIGINT, Logger::logHandler);
    signal(SIGHUP, Logger::logHandler);
}

void Logger::writeFile()
{
    std::cout << endl;
    std::cout << "got " << instance.pixels.size() << " pixels for "<< outputFile << std::endl;
    std::cout << "got " << instance.intersections.size() << " intersections for "<< outputFile << std::endl;

    // write pixels out to file
    std::ofstream out(outputFile);
    std::vector<Pixel>::iterator px;
    std::vector<Intersection>::iterator i;
    for( px = instance.pixels.begin(), i = instance.intersections.begin(); px < instance.pixels.end(), i < instance.intersections.end(); px++, i++)
    {
        PrintPixel(out, *px);
        PrintIntersection(out, *i);
    }
    out.close();

    instance.pixels.clear();
    instance.intersections.clear();

    usleep(100*1000); // slow re-joiner problem?
}

void Logger::signalHandler( int signum )
{
#ifdef DEBUG
    cout << "Interrupt signal (" << signum << ") received.\n";
#endif /* DEBUG */
    writeFile();

   exit(signum);
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 5 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Logger log(argv[1], argv[2], argv[3], "", "");
    sprintf(log.outputFile, "%s/data/o%s", BASEDIR, argv[4]);
    // register signal SIGINT and signal handler
    log.registerHandler();

	cout << "running" << endl;
	log.run();

	cout << "shutting down" << endl;
	return 0;
}
