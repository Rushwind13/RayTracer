//============================================================================
// Name        : Background.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
using namespace std;
#include "Background.hpp"
#include "Lighting.hpp"

void Background::local_setup()
{

}

// You will get here if a Shadow test registers a hit with an object between the intersection and a particular light
bool Background::local_work(byte_vector *header, byte_vector *payload)
{
	std::cout << "doing work... ";
	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	// TODO: get this from the World configuration
	pixel.color = Color(0.2,0,0.23);

	// TODO: use altitude of original ray to fade the background color.
	float fade = dot( vec3(0,0,-1), pixel.primaryRay.direction );
	// multiply pixel.color by this (you get full color at the horizon, fading to black at zenith)
	pixel.color *= fade;

	pixel.gothit = false;

	encodeBuffer( header, (void *)&pixel, sizeof(Pixel));
	payload->clear();

	return true; // send an outbound message as a result of local_work()
}

void Background::local_shutdown()
{
	std::cout << "shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;

	// This will have several publisher outputs:
	// - Shadow intersection tests for each light
	// - Reflection test
	// - Refraction test
	// Finally, send off Ambient and Emissive color value.
	Background bak("Black", "BKG", "ipc:///tmp/feeds/broadcast", "COLOR", "ipc:///tmp/feeds/control");

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
