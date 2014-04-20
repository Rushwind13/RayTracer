/*
 * Lit.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef LIT_HPP_
#define LIT_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Lit : public Widget
{
public:
	Lit( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~Lit(){};

protected:
	virtual void local_setup();
	virtual bool local_work( byte_vector *header, byte_vector *payload );
	virtual void local_shutdown();

private:
	World world;
};


#endif /* LIT_HPP_ */
