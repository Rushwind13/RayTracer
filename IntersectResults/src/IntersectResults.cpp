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
//#define DEBUG
	std::cout << "IntersectResults starting up... ";
}

bool IntersectResults::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ") " << pixel.type << " ";
#endif /* DEBUG */

	Intersection i;
	msgpack::object obj2;
	unPackPart( payload, &obj2 );
	obj2.convert( i );
#ifdef DEBUG
	std::cout << i.oid << " " << i.gothit;
#endif /* DEBUG */

	bool testComplete = false;
	testComplete = storeIntersection( pixel, i );

	if( testComplete )
	{
		// You got a full set of responses for this test, so
		// grab the best result...
		int64_t key = hash( pixel );
		i = nearest[key];
		if( pixel.type == iPrimary ) pixel.oid = i.oid;
#ifdef DEBUG
        std::cout << "Test Complete " << i.distance[0];
#endif /* DEBUG */

		// Prepare payload for sending to next stage...
		header->clear();
		msgpack::pack( header, pixel );
		payload->clear();
		msgpack::pack( payload, i );

		// and clean up local hash tables.
		nearest.erase(key);
		response_count.erase(key);
	}
#ifdef DEBUG
	std::cout << std::endl;
#endif /* DEBUG */

	return testComplete; // if true, send an outbound message as a result of local_work()
}

bool IntersectResults::storeIntersection( Pixel pixel, Intersection hit )
{
	int64_t key = hash( pixel );
	int16_t count;
	Intersection curr_nearest;
	bool testComplete = false;

	//std::cout << " k:" << key;

	if( response_count.find(key) != response_count.end())
	{
		// You already have a response for this object; decide what to do with the new hit and bump the response count
		count = response_count[key];
		count++;

		if( hit.gothit )
		{
			curr_nearest = nearest[key];
			//std::cout << " had " << curr_nearest.gothit;

			if( pixel.type == iShadow )
			{
				// Shadow tests only need the first intersected object, not the nearest
				if( !curr_nearest.gothit )
				{
					nearest[key] = hit;
				}
			}
			else
			{
				// If the new hit is closer, keep it.
				if( hit.distance[0] < curr_nearest.distance[0] )
				{
					nearest[key] = hit;
				}
			}
		}
	}
	else
	{
		//std::cout << " new " << hit.gothit;
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

void IntersectResults::local_send( msgpack::sbuffer *header, msgpack::sbuffer *payload )
{
	// Depending on whether it is a hit or a miss, and depending upon the test type,
	// this function will publish the intersection to one of 4 places.
	// 			HIT				MISS
	// SHADOW	BLACK			LIT
	// other	SHADE			BKG

	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );

	Intersection i;
	unPackPart( payload, &obj );
	obj.convert( i );

	char pub[6] = "";

	if( pixel.type == iShadow )
	{
		//std::cout << " shadow test ";
		// Shadow rays get "Black" when they hit something, or "Lit" when they miss
		strcpy(pub, (i.gothit && (i.distance[0] < pixel.distance)) ? "BLACK":"LIT");
		pixel.gothit = true;
	}
	else
	{
		// All other ray types (Primary, Reflection, Refraction)
		// get "Shade" when they hit something, or "Background" when they miss
		strcpy( pub, i.gothit ? "SHADE":"BKG");
	}

#ifdef DEBUG
	if(pixel.type == iShadow)//strcmp(pub, "BLACK") == 0 )
	{
		std::cout << "(" << pixel.x << "," << pixel.y << ") " << pub << " " << response_count.size() << std::endl;
	}
	if( pixel.x == 0.0f && pixel.y == 0.0f)
	{
		std::cout << "got 0,0 " << pub << std::endl;
	}
#endif /* DEBUG */

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
    if( argc != 6 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	IntersectResults ir(argv[1], argv[2], argv[3], argv[4], argv[5]);
	cout << "running" << endl;
	ir.run();

	cout << "shutting down" << endl;
	return 0;
}
