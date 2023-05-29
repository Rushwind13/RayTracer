//============================================================================
// Name        : IntersectWith.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Object intersection widget. One instance of this runs per world object, takes in a ray and
//				 returns an Intersection with position where that ray intersects this object (or not).
//============================================================================

#include <iostream>
#include <unistd.h>
using namespace std;
#include "IntersectWith.hpp"
#include "Object.hpp"
#include "Pixel.hpp"
#include "glm/glm.hpp"

void IntersectWith::local_setup()
{
	// Slow joiner problem
	usleep(100*1000);
#define DEBUG
	object = world.FindObject( world_object );
	assert( object );
    pixel_count = 0;
}

bool IntersectWith::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	assert(object);
	Pixel pixel;
	Intersection i;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );

    if( pixel.type == iInvalid )
    {
        running = false;
        std::cout << "received EOF after " << pixel_count << " pixels, passing it along...";

        header->clear();
        payload->clear();
        msgpack::pack( header, pixel );
        msgpack::pack( payload, i );
        PrintPixel(cout, pixel);

        sendMessage(header, payload);
        std::cout << "sent." << std::endl;
        pixel_count = 0;
        usleep(100*1000); // slow re-joiner problem?
        return false;
    }

    pixel_count++;
    std::cout << "(" << pixel.y << ")" << "\r";

	assert(object);

	// Primary rays don't have a payload,
	// but Shadow rays will.
	// TODO: Reflection and Refraction ones might, too.
	assert(object);
	if( pixel.type == iShadow )
	{
		i.distance[0] = i.distance[1] = pixel.distance;
		i.anyhit = true;

#ifdef DEBUG
        // std::cout << " shadow " << "d: " << i.distance[0];
#endif /* DEBUG */
	}
	assert(object);

	bool gothit;
	assert( object );

	// Shadow tests do not allow self-intersection
	if( pixel.type == iShadow && pixel.oid == object->oid )
	{
#ifdef DEBUG
		std::cout << " no self-shadow ";
#endif /* DEBUG */
		i.gothit = false;
	}
	else
	{
		//std::cout << "(" << pixel.x << "," << pixel.y << ") " << pixel.type << " ";
		//printvec( "sent o", pixel.r.origin );
		//printvec( "sent d", pixel.r.direction );
		gothit = object->Intersect( pixel.r, i );
		i.gothit = gothit;
#ifdef DEBUG
		std::string test = "Intersect ";
		if( pixel.type == iShadow )
		{
			test =  "Shadowed ";
		}
		if( gothit )
		{
			//std::cout << " h: " << i.distance;
			std::cout <<  test << object->name << " at (" << pixel.x << "," << pixel.y << ") depth:" << pixel.depth << std::endl;
            PrintIntersection(cout, i);
		}
#endif /* DEBUG */
	}

    payload->clear();
	msgpack::pack( payload, i );

    usleep(2*1000);
	return true; // send an outbound message as a result of local_work()
}

void IntersectWith::local_shutdown()
{
	std::cout << "shutting down... ";
	//delete( object );
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 7 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	IntersectWith iw(argv[1], argv[2], argv[3], argv[4], argv[5]);
    iw.world_object = argv[6];
	cout << "running" << endl;
	iw.run();

	cout << "shutting down" << endl;
	return 0;
}
