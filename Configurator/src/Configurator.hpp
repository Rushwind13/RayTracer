/*
 * Configurator.hpp
 *
 *  Created on: Apr 25, 2023
 *      Author: jiharris
 */

#ifndef CONFIGURATOR_HPP_
#define CONFIGURATOR_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Configurator : public Widget
{
public:
	Configurator( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint, false, true )
	{
	};
	~Configurator(){};

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();

private:
};


#endif /* CONFIGURATOR_HPP_ */
