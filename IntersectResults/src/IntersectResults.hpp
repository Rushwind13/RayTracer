/*
 * IntersectResults.hpp
 *
 *  Created on: Apr 5, 2014
 *      Author: jiharris
 */

#ifndef INTERSECTRESULTS_HPP_
#define INTERSECTRESULTS_HPP_
#include "Widget.hpp"
#include "World.hpp"

class IntersectResults : public Widget
{
public:
	IntersectResults( char *_name, char *_subscription, char *_sub_endpoint, char *_publication, char *_pub_endpoint ) :
		Widget( _name, _subscription, _sub_endpoint, _publication, _pub_endpoint )
	{
	};
	~IntersectResults(){};

protected:
	virtual void local_setup();
	virtual bool local_work( byte_vector *header, byte_vector *payload );
	virtual void local_shutdown();

private:
	World world;
};


#endif /* INTERSECTRESULTS_HPP_ */
