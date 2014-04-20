/*
 * Black.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef BLACK_HPP_
#define BLACK_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Black : public Widget
{
public:
	Black( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~Black(){};

protected:
	virtual void local_setup();
	virtual bool local_work( byte_vector *header, byte_vector *payload );
	virtual void local_shutdown();

private:
};


#endif /* BLACK_HPP_ */
