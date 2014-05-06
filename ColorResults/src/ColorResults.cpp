//============================================================================
// Name        : ColorResults.cpp
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
#include "ColorResults.hpp"
#include "Object.hpp"
#include "glm/glm.hpp"

void ColorResults::local_setup()
{
	std::cout << "IntersectResults starting up... ";
}

bool ColorResults::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( &pixel );
	std::cout << "(" << pixel.x << "," << pixel.y << ")";

	bool colorComplete = false;
	colorComplete = storeColor( pixel );

	if( colorComplete )
	{
		// You got a full set of responses for this test, so
		// grab the result...
		// TODO: This is a bug -- need to aggregate all depths and all types at this point.
		int64_t key = hash( pixel );
		pixel.color = accumulator[key];

		// Prepare payload for sending to next stage...
		payload->clear();
		header->clear();

		msgpack::pack( header, pixel );
		printvec("c", pixel.color);

		// and clean up local hash tables.
		accumulator.erase(key);
		response_count.erase(key);/**/
	}

	std::cout << std::endl;
	return colorComplete; // if true, send an outbound message as a result of local_work()
}

bool ColorResults::storeColor( Pixel pixel )
{
	int64_t key = hash( pixel );
	int16_t count;
	Color curr_accumulator;
	bool testComplete = false;

	// The states you could be in:
	// Primary miss - you get only one response from "bkg"
	// Primary hit - you get one response from Shader and one per light from Shadow
	// Shadow miss - this should be coming from "lit"
	// Shadow hit - this should be coming from "black"
	// TODO: add reflection and refraction states (will likely have to deal with depth in addition to the rest)

	if( pixel.gothit == false )
	{
		std::cout << "no hit " << pixel.type;
		// you know that this is the only one you're getting (at least for this depth).
		accumulator[key] = pixel.color;
		testComplete = true;

		return testComplete; // TODO: don't know if you can easy-out like this when recursive...
	}

	// otherwise, you are expecting 1 + light_count responses, so accumulate until you have them all
	if( response_count.find(key) != response_count.end())
	{
		std::cout << "already had this one ";
		// You already have a response for this object; accumulate the new color and bump the response count
		count = response_count[key];
		count++;

		// Shadow tests only need the first intersected object, not the nearest
		curr_accumulator = accumulator[key];
		printvec("a", curr_accumulator);
		// If the new hit is closer, keep it.
		Color outcolor;
		outcolor = pixel.color + curr_accumulator;
		if( outcolor.r > 1.0 ) outcolor.r = 1.0;
		if( outcolor.g > 1.0 ) outcolor.g = 1.0;
		if( outcolor.b > 1.0 ) outcolor.b = 1.0;

		accumulator[key] = outcolor;
		printvec("p", pixel.color);
		printvec("o", outcolor);
	}
	else
	{
		std::cout << "new one ";
		count = 1;
		accumulator[key] = pixel.color;
		printvec("p", pixel.color);
	}

	// want one more than the number of lights (one from the basic hit and one per light, or just one total if it's a miss)
	if( count < world.light_count + 1 )
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

/*void ColorResults::local_send( msgpack::sbuffer *header, msgpack::sbuffer *payload )
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
}/**/

void ColorResults::local_shutdown()
{
	std::cout << "IntersectResults shutting down... ";
	response_count.clear();
	accumulator.clear();
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
	ColorResults cr("IntersectWith", "COLOR", "ipc:///tmp/feeds/broadcast", "PNG", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//iw.world_object = argv[1];
	}
	cout << "running" << endl;
	cr.run();

	cout << "shutting down" << endl;
	return 0;
}