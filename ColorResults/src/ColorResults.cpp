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
#include <unistd.h>
using namespace std;
#include "ColorResults.hpp"
#include "Object.hpp"
#include "glm/glm.hpp"

void ColorResults::local_setup()
{
//#define DEBUG
	std::cout << "ColorResults starting up... ";
    pixel_count = 0;
}

bool ColorResults::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
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
        msgpack::pack( payload, i );
        // Optional forwarding log when SMOKE_MODE is enabled
        const char* smoke = getenv("SMOKE_MODE");
        bool smoke_mode = (smoke && smoke[0] != '\0' && smoke[0] != '0');
#if !defined(DEBUG)
        if (smoke_mode)
        {
            std::cout << "ColorResults -> DEPTH (x=" << pixel.x << ", y=" << pixel.y << ", d=" << pixel.depth << ")" << std::endl;
        }
#endif /* !DEBUG */
#ifdef DEBUG
		printvec("c", pixel.color);
#endif /* DEBUG */

		// and clean up local hash tables.
		accumulator.erase(key);
		response_count.erase(key);/**/
	}

#ifdef DEBUG
	if( pixel.gothit ) std::cout << std::endl;
#endif /* DEBUG */
	return colorComplete; // if true, send an outbound message as a result of local_work()
}

bool ColorResults::storeColor( Pixel pixel )
{
	int64_t key = hash( pixel );
	int16_t count;
	Color curr_accumulator;
	bool testComplete = false;
	const char* smoke = getenv("SMOKE_MODE");
	bool smoke_mode = (smoke && smoke[0] != '\0' && smoke[0] != '0');

	// The states you could be in:
	// Primary miss - you get only one response from "bkg" (gothit = F)
	// Primary hit - you get one response from Shader and one per light from Shadow (gothit = T, sum all these)
	// Shadow miss - this should be coming from "lit"
	// Shadow hit - this should be coming from "black"
	// TODO: add reflection and refraction states (will likely have to deal with depth in addition to the rest)

	if( pixel.gothit == false )
	{
#ifdef DEBUG
		std::cout << "no hit " << pixel.type;
#endif /* DEBUG */
		// you know that this is the only one you're getting (at least for this depth).
		accumulator[key] = pixel.color;
		testComplete = true;

		return testComplete; // TODO: don't know if you can easy-out like this when recursive...
	}

	// otherwise, you are expecting 1 + light_count responses, so accumulate until you have them all
	if( response_count.find(key) != response_count.end())
	{
#ifdef DEBUG
		std::cout << "already had this one ";
#endif /* DEBUG */
		// You already have a response for this object; accumulate the new color and bump the response count
		count = response_count[key];
		count++;

		// you need contrib from each light
		curr_accumulator = accumulator[key];
#ifdef DEBUG
		printvec("a", curr_accumulator);
#endif /* DEBUG */
		// mix the new color into the existing accumulator, without overflowing
		Color outcolor;
		outcolor = pixel.color + curr_accumulator;
		if( outcolor.r > 1.0 ) outcolor.r = 1.0;
		if( outcolor.g > 1.0 ) outcolor.g = 1.0;
		if( outcolor.b > 1.0 ) outcolor.b = 1.0;

		accumulator[key] = outcolor;
#ifdef DEBUG
		printvec("p", pixel.color);
		printvec("o", outcolor);
#endif /* DEBUG */
	}
	else
	{
#ifdef DEBUG
		std::cout << "new one " << key << " " << pixel.x << " " << pixel.y << " " << pixel.depth << " ";
#endif /* DEBUG */
		count = 1;
		accumulator[key] = pixel.color;
#ifdef DEBUG
		printvec("p", pixel.color);
#endif /* DEBUG */
	}

	// want one more than the number of lights (one from the basic hit and one per light, or just one total if it's a miss)
	// In SMOKE_MODE, allow completion after the first contribution to keep the pipeline flowing
	int16_t threshold = smoke_mode ? 1 : (world.light_count + 1);
	if( count < threshold )
	{
#ifdef DEBUG
        std::cout << count << std::endl;
#endif /* DEBUG */
		response_count[key] = count;
		testComplete = false;
	}
	else
	{
#ifdef DEBUG
        std::cout << "finished" << std::endl;
#endif /* DEBUG */
		testComplete = true;
	}

	return testComplete;
}

void ColorResults::local_shutdown()
{
	std::cout << "ColorResults shutting down... ";
	response_count.clear();
	accumulator.clear();
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 6 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
    ColorResults cr(argv[1], argv[2], argv[3], argv[4], argv[5]);

	cout << "running" << endl;
	cr.run();

	cout << "shutting down" << endl;
	return 0;
}
