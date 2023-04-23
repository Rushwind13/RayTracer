/*
 * Reflection.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef REFLECTION_HPP_
#define REFLECTION_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Reflection : public Widget
{
public:
	Reflection( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint, true, true )
	{
	};
	~Reflection(){};

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();

private:
	World world;
};


#endif /* REFLECTION_HPP_ */
