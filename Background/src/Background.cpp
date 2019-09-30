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
bool Background::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );

	// TODO: get this from the World configuration
	//pixel.color = Color(0.2,0,0.23);
	pixel.color = Color(0.8,0,0.73);
	//pixel.color = Color(0.25,0.25,0.25);
	//pixel.color = Color(0.0,0.8,0.0);

    // Below horizon, full background color.
	float fade = 1.0;

    // Above horizon, fade from background color to black from horizon to zenith
    if( pixel.primaryRay.direction.y > 0.0 )
    // multiply pixel.color by this (you get full color at the horizon, fading to black at zenith)
        fade = glm::dot( glm::vec3(0,0,-1), pixel.primaryRay.direction );


	pixel.color *= fade;

	pixel.gothit = false;

	header->clear();

	msgpack::pack( header, pixel );
	payload->clear();

	Pixel pix2;
	msgpack::object obj2;
	unPackPart( header, &obj2 );
	obj2.convert( pix2 );

#define DEBUG // 2019 09 29 Happy birthday, Melissa :)
#ifdef DEBUG
	std::cout << "(" << std::setw(3) << pix2.x << "," << std::setw(3) << pix2.y << ") ";
	printvec("c", pix2.color);
	std::cout << std::endl;
#endif /* DEBUG */

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
