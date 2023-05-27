/*
 * Logger.hpp
 *
 *  Created on: Apr 25, 2023
 *      Author: jiharris
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_
#include <time.h>
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Logger : public Widget
{
public:
	Logger( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint, true, true )
	{
	};
	Logger(): Widget( "", "", "", "", "", false, false ){};
	~Logger(){};
    void signalHandler( int signum );
    static void logHandler (int signum)
    {
        instance.signalHandler(signum);
    };
    void registerHandler();
    char outputFile[255];

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();

private:
    void writeFile();

    static Logger instance;
    std::vector<Pixel> pixels;
    std::vector<Intersection> intersections;
    clock_t latest_write;
};


#endif /* LOGGER_HPP_ */
