/*
 * ColorResults.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef COLORRESULTS_HPP_
#define COLORRESULTS_HPP_
#include "Widget.hpp"
#include "World.hpp"
#include "Pixel.hpp"
#include <map>

class ColorResults : public Widget
{
public:
	ColorResults( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~ColorResults(){};

protected:
	virtual void local_setup();
	virtual bool local_work( byte_vector *header, byte_vector *payload );
	virtual void local_shutdown();
	virtual void local_send( byte_vector *header, byte_vector *payload );

private:
	World world;

	// Simple hashing function
	// creates a hash of x,y , the depth, and the test type
	// assumes that there will not be more than 9,999 pixels in either dimension
	// assumes that there will not be more than 99 test types or recursion depth
	int64_t hash( Pixel pixel )
	{
		int64_t hash = 0;
		hash += (int64_t)pixel.x * 10000 * 100 * 100;
		hash += (int64_t)pixel.y * 100 * 100;
		hash += (int64_t)pixel.depth * 100;
		hash += (int64_t)pixel.type;

		return hash;
	}

	std::map<int64_t, int16_t> response_count;
	std::map<int64_t, Color> accumulator;

	bool storeColor( Pixel pixel );
};


#endif /* COLORRESULTS_HPP_ */
