/*
 * Feeder.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef FEEDER_HPP_
#define FEEDER_HPP_
#include "Widget.hpp"

class Feeder : public Widget
{
public:
	Feeder( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~Feeder(){};

    char inputFile[255];

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();
	virtual void local_send( msgpack::sbuffer *header, msgpack::sbuffer *payload ) {};
private:
};


#endif /* FEEDER_HPP_ */
