//============================================================================
// Name        : testIntersectWith.cpp
// Author      : JH
// Version     :
// Copyright   : Copyright 2019 Jimbo S. Harris. All rights reserved.
// Description : tests IntersectWith by:
//             : generating known rays on INTERSECT, and
//             : receiving the output on RESULT
//============================================================================

#include <iostream>
using namespace std;
#include "Lit.hpp"
#include "Lighting.hpp"

void testIntersectWith::local_setup()
{
	// Slow joiner problem
	usleep(100*1000);

	camera.setup();

// TODO: Ray creation done as a function. Probably needs to go in the local_work,
//       but how to iterate over a list? shrug
	Pixel pixel;
	Pixel outpx;
	msgpack::sbuffer header(0);
	msgpack::sbuffer pay(0);

    // First, the (0,0) case
    pixel.x = 0.0f;
    pixel.y = 0.0f;
    pixel.r.origin=glm::vec3(0.0);
    pixel.r.direction=glm::vec3(0.0,0.0,-1.0);
    pixel.primaryRay = pixel.r;
    pixel.type = iPrimary;
    pixel.weight = 1.0f;
    pixel.depth = 0;

    // TODO: Scenarios to test:
    // sphere radius 1, 2 units away, expect intersection at 1 unit with normal (0,0,1)
    // move sphere +1 unit in Y, expect miss (but a near (0,-1,0) normal if close)
    // move sphere away, same normal but different intersect point
    // radius 2 sphere, 3 units away...
    // Need to be able to feed different world models to the test target

	header.clear();
	msgpack::pack( header, pixel );

	sendMessage(&header, &pay);


	/*for( int j = 130; j < 140; j++ )
	{
		for( int i = 205; i < 215; i++ )
		{/**/
	for( int j = 0; j < camera.height; j++ )
	{
		for( int i = 0; i < camera.width; i++ )
		{/**/
			std::cout << "Pixel (" << i << "," << j << "): ";
			pixel.x = (float)i * 1.0f;
			pixel.y = (float)j * 1.0f;
			pixel.r = camera.RayThroughPoint(pixel.x, pixel.y);
			pixel.primaryRay = pixel.r;
			pixel.type = iPrimary;
			pixel.weight = 1.0f;
			pixel.depth = 0;

			printvec( "o", pixel.r.origin);
			printvec( "d", pixel.r.direction);

			header.clear();
			msgpack::pack( header, pixel );

			sendMessage(&header, &pay);
			std::cout << "\r";
			usleep(1*1000);
		}
	}
	std::cout << std::endl;
}

// You will get here if a Shadow test registers a miss with an object between the intersection and a particular light
// so this point is lit by this light.
bool testIntersectWith::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ") ";
#endif /* DEBUG */

	// We know we have at least one hit now, so...
	pixel.gothit = true;

	Intersection i;
	msgpack::object obj2;
	unPackPart( payload, &obj2 );
	obj2.convert( i );
	payload->clear();


// TODO: Create one (or, eventually, several)
//       known rays, with known expected Intersection results


	//std::cout << std::endl;
	return true; // send an outbound message as a result of local_work()
}

void testIntersectWith::local_shutdown()
{
	std::cout << "shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;

	Lit bak("testIntersectWith", "RESULT", "ipc:///tmp/feeds/broadcast", "INTERSECT", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//sh.world_object = argv[1];
	}
	cout << "running" << endl;
	bak.run();

	cout << "shutting down" << endl;
	return 0;
}
