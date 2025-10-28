/*
 * PixelFactory.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef PIXELFACTORY_HPP_
#define PIXELFACTORY_HPP_
#include "Widget.hpp"
#include "Camera.hpp"
#include "Pixel.hpp"

class PixelFactory : public Widget
{
public:
	PixelFactory( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint, false, true )
	{
	};
	~PixelFactory(){};

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload ) { return false; };
	virtual void local_shutdown(){};

private:
	Camera camera;
    std::vector<Pixel> pixels;
};


#endif /* PIXELFACTORY_HPP_ */
