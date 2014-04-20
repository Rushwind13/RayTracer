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
#include "Object.hpp"
#include "Pixel.hpp"
#include "glm/glm.hpp"

void IntersectWith::local_setup()
{
	object = world.FindObject( world_object );
	/*//	create world object
	if( world_object.compare("sphere1") == 0 )
	{
		vec3 center(-2.5,0.0,-5.0);
		float radius=2.0;
		object = new Sphere(center, radius);
		object->oid = 1;
		object->name = "sphere1";
	}
	else if( world_object.compare("sphere2") == 0 )
	{
		vec3 center2(2.5,0.0,-10.0);
		float radius2=4.0;
		object = new Sphere(center2, radius2);
		object->color = Color(0.0,0.0,1.0);
		object->oid = 2;
		object->name = "sphere2";
	}/**/
}

bool IntersectWith::local_work(byte_vector *header, byte_vector *payload)
{
	std::cout << "doing work... ";
	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	// Primary rays don't have a payload,
	// but Shadow rays will.
	// TODO: Reflection and Refraction ones might, too.
	Intersection i;
	if( pixel.type == iShadow )
	{
		//memcpy( payload, &i, sizeof( Intersection ));
		i.distance = pixel.distance;
		i.anyhit = true;
	}

	bool gothit;
	i.gothit = object->Intersect( pixel.r, i );

	unsigned char *buffer = (unsigned char *)malloc(sizeof( Intersection ) );
	memcpy( &i, buffer, sizeof( Intersection ));

	payload->insert(payload->begin(), buffer, buffer + (sizeof( Intersection )) );

	return true; // send an outbound message as a result of local_work()
}

void IntersectWith::local_shutdown()
{
	std::cout << "shutting down... ";
	delete( object );
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
