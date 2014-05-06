/*
 * Writer.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef WRITER_HPP_
#define WRITER_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Camera.hpp"
#include "Pixel.hpp"
#include <map>

class Writer : public Widget
{
public:
	Writer( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~Writer(){};
	World world;

protected:
	virtual void local_setup();
	virtual bool local_work( msgpack::sbuffer *header, msgpack::sbuffer *payload );
	virtual void local_shutdown();
	virtual void local_send( msgpack::sbuffer *header, msgpack::sbuffer *payload ) {};

private:
	Camera camera;
	Color *image;
	int32_t pixel_count;

	void SaveImage();

	std::map<int64_t, int16_t> response_count;
	std::map<int64_t, Color> accumulator;

	bool storePixel( Pixel pixel );
};


#endif /* WRITER_HPP_ */
