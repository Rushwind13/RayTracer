/*
 * Background.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef BACKGROUND_HPP_
#define BACKGROUND_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Background : public Widget
{
public:
	Background( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~Background(){};

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();

private:
};


#endif /* BACKGROUND_HPP_ */
