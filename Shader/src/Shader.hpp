/*
 * Shader.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef SHADER_HPP_
#define SHADER_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"

class Shader : public Widget
{
public:
	Shader( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint, true, false )
	{
	};
	~Shader(){};

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();
	zmq::socket_t *black_publisher;
	zmq::socket_t *intersect_publisher;

private:
	World world;
    int pixel_count;
	void prepareShadowTest( Pixel *pixel, const Intersection i );
};


#endif /* SHADER_HPP_ */
