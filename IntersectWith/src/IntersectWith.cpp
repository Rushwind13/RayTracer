//============================================================================
// Name        : IntersectWith.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Object intersection widget. One instance of this runs per world object, takes in a ray and
//				 returns an Intersection with position where that ray intersects this object (or not).
//============================================================================

#include <iostream>
using namespace std;
#include "IntersectWith.hpp"
#include "Pixel.hpp"
#include "glm/glm.hpp"

void IntersectWith::local_setup()
{
	object = world.FindObject( world_object );
	assert( object );
}

bool IntersectWith::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	assert(object);
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );
	//if(pixel.type == iShadow) std::cout << "(" << pixel.x << "," << pixel.y << ")" << pixel.type << " ";
	assert(object);

	// Primary rays don't have a payload,
	// but Shadow rays will.
	// TODO: Reflection and Refraction ones might, too.
	Intersection i;
	assert(object);
	if( pixel.type == iShadow )
	{
		//std::cout << " shadow ";
		i.distance = pixel.distance;
		i.anyhit = true;

		//std::cout << "d: " << i.distance;
	}
	assert(object);

	bool gothit;

<<<<<<< HEAD
    if(pixel.depth >= world.maxdepth*2)
    {
        std::cout << "(" << std::setw(3) << pixel.x << "," << std::setw(3) << pixel.y << ")" << pixel.type << " exceeds maxdepth; no further intersection allowed." << std::endl;
        return true;
    }
=======
    // if(pixel.depth >= world.maxdepth*2)
    // {
    //     std::cout << "(" << std::setw(3) << pixel.x << "," << std::setw(3) << pixel.y << ")" << pixel.type << " exceeds maxdepth; no further intersection allowed." << std::endl;
		// 		pixel.color = glm::vec3(0.1, 1.0, 1.0);
    //     return true;
    // }
>>>>>>> c6008478dadfa3cbd0809562ba9e7945bf89e0b0


	// Shadow tests do not allow self-intersection
	if( pixel.type == iShadow && pixel.oid == object->oid )
	{
		//std::cout << " no self-shadow ";
		i.gothit = false;
	}
	else
	{
		//std::cout << "(" << pixel.x << "," << pixel.y << ") " << pixel.type << " ";
		//printvec( "sent o", pixel.r.origin );
		//printvec( "sent d", pixel.r.direction );
		gothit = object->Intersect( pixel.r, i );
		i.gothit = gothit;
		std::string test = "Intersect ";
		if( pixel.type == iShadow )
		{
			test =  "Shadowed ";
		}
		if( gothit )
		{
			//std::cout << " h: " << i.distance;
			std::cout <<  test << object->name << " at (" << std::setw(3) << pixel.x << "," << std::setw(3) << pixel.y << ") depth:" << pixel.depth << std::endl;
		}
	}

	msgpack::pack( payload, i );
	//if( pixel.type == iShadow )std::cout << std::endl;
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
	IntersectWith iw("IntersectWith", "INTERSECT", "ipc:///tmp/feeds/broadcast", "RESULT", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		iw.world_object = argv[1];
	}
	cout << "running" << endl;
	iw.run();

	cout << "shutting down" << endl;
	return 0;
}
