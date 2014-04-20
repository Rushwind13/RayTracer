//============================================================================
// Name        : IntersectResults.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Stateful Intersection results hub.
//					- listens for world objects to complete intersection tests,
//					- waits until all objects have returned results
//					- decides upon nearest intersection (if necessary)
//					- passes completed result to either Shader, Backgrounder, Shadow, or Lit widgets for processing
//============================================================================

#include <iostream>
using namespace std;
#include "IntersectResults.hpp"
#include "Object.hpp"
#include "glm/glm.hpp"

void IntersectResults::local_setup()
{
	std::cout << "IntersectResults starting up... ";
}

bool IntersectResults::local_work(byte_vector *header, byte_vector *payload)
{
	std::cout << "doing work... ";
	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	Intersection i;
	memcpy( payload, &i, sizeof(Intersection) );

	bool testComplete = false;
	testComplete = storeIntersection( pixel, i );

	if( testComplete )
	{
		// You got a full set of responses for this test, so
		// grab the best result...
		int64_t key = hash( pixel );
		i = nearest[key];

		// Prepare payload for sending to next stage...
		payload->clear();
		unsigned char *buffer = (unsigned char *)malloc(sizeof( Intersection ) );
		memcpy( &i, buffer, sizeof( Intersection ));

		payload->insert(payload->begin(), buffer, buffer + (sizeof( Intersection )) );

		// and clean up local hash tables.
		nearest.erase(key);
		response_count.erase(key);
	}

	return testComplete; // if true, send an outbound message as a result of local_work()
}

bool IntersectResults::storeIntersection( Pixel pixel, Intersection hit )
{
	int64_t key = hash( pixel );
	int16_t count;
	Intersection curr_nearest;
	bool testComplete = false;

	if( response_count.find(key) != response_count.end())
	{
		// You already have a response for this object; decide what to do with the new hit and bump the response count
		count = response_count[key];
		count++;

		// Shadow tests only need the first intersected object, not the nearest
		if( pixel.type != iShadow && hit.gothit )
		{
			curr_nearest = nearest[key];
			// If the new hit is closer, keep it.
			if( hit.distance < curr_nearest.distance )
			{
				nearest[key] = hit;
			}
		}
	}
	else
	{
		count = 1;
		nearest[key] = hit;
	}

	if( count < world.object_count )
	{
		response_count[key] = count;
		testComplete = false;
	}
	else
	{
		testComplete = true;
	}

	return testComplete;
}

void IntersectResults::local_send( byte_vector *header, byte_vector *payload )
{
	// Depending on whether it is a hit or a miss, and depending upon the test type,
	// this function will publish the intersection to one of 4 places.
	// 			HIT				MISS
	// SHADOW	BLACK			LIT
	// other	SHADE			BKG

	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	Intersection i;
	memcpy( payload, &i, sizeof(Intersection) );

	char pub[6] = "";

	if( pixel.type == iShadow )
	{
		// Shadow rays get "Black" when they hit something, or "Lit" when they miss
		strcpy(pub, (i.gothit && (i.distance < pixel.distance)) ? "BLACK":"LIT");
	}
	else
	{
		// All other ray types (Primary, Reflection, Refraction)
		// get "Shade" when they hit something, or "Background" when they miss
		strcpy( pub, i.gothit ? "SHADE":"BKG");
	}

	sendMessage( header, payload, pub );
}

void IntersectResults::local_shutdown()
{
	std::cout << "IntersectResults shutting down... ";
	response_count.clear();
	nearest.clear();
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
	IntersectResults iw("IntersectWith", "RESULT", "ipc:///tmp/feeds/broadcast", "SHADE", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//iw.world_object = argv[1];
	}
	cout << "running" << endl;
	iw.run();

	cout << "shutting down" << endl;
	return 0;
}
