//============================================================================
// Name        : Logger.cpp
// Author      : JH
// Version     :
// Copyright   : Copyright 2023 Jimbo S. Harris. All rights reserved.
// Description : Capture all messages from a particular channel
//============================================================================

#include <csignal>
#include <iostream>
#include <algorithm>
#include <unistd.h>
using namespace std;
#include "Logger.hpp"
#include "Lighting.hpp"

Logger Logger::instance;
Logger* Logger::active = nullptr;

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

    // Payload may be missing (e.g., capturing PixelFactory stream). Fall back to default Intersection.
    Intersection i;
    try {
        msgpack::object obj2;
        unPackPart( payload, &obj2 );
        obj2.convert( i );
    } catch (const std::exception&) {
        // leave i as default: gothit=false, distance=1e9
    }

    if( pixel.type != iInvalid )
    {
        pixels.push_back(pixel);
        intersections.push_back(i);
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
    std::cout << "got " << pixels.size() << " pixels for "<< outputFile << std::endl;
    std::cout << "got " << intersections.size() << " intersections for "<< outputFile << std::endl;

    // write pixels out to file (optionally append)
    std::ofstream out;
    const char* append_env = std::getenv("LOG_APPEND");
    if (append_env && *append_env && *append_env != '0') {
        out.open(outputFile, std::ios::app);
    } else {
        out.open(outputFile);
    }

    size_t n = std::min(pixels.size(), intersections.size());
    for (size_t idx = 0; idx < n; ++idx) {
        PrintPixel(out, pixels[idx]);
        PrintIntersection(out, intersections[idx]);
    }
    out.close();

    pixels.clear();
    intersections.clear();

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
    Logger::active = &log;

    // Allow overriding the base directory for log output at runtime via LOG_BASEDIR
    const char* log_basedir = std::getenv("LOG_BASEDIR");
    if (!log_basedir || !*log_basedir) {
        log_basedir = BASEDIR; // compile-time default
    }
    snprintf(log.outputFile, sizeof(log.outputFile), "%s/o%s", log_basedir, argv[4]);

    // Special case for stepwise Stage 1: bind the SUB socket to capture a PUB-only bus
    const char* bind_flag = std::getenv("LOGGER_BIND");
    if (bind_flag && *bind_flag && *bind_flag != '0') {
        // Switch subscriber mode from connect to bind
        log.bindSubscriber();
    }

    // register signal SIGINT and signal handler
    log.registerHandler();

    cout << "running" << endl;
    log.run();

    cout << "shutting down" << endl;
    return 0;
}
