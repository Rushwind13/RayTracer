//============================================================================
// Name        : Black.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
using namespace std;
#include "Black.hpp"
#include "Lighting.hpp"

void Black::local_setup()
{

}

// You will get here if a Shadow test registers a hit with an object between the intersection and a particular light
// So this point is shadowed by this light.
bool Black::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( &pixel );
	std::cout << "(" << pixel.x << "," << pixel.y << ")";

	pixel.color = Color(0.0f,0.0f,0.0f);
	printvec("c", pixel.color);

	header->clear();
	msgpack::pack( header, pixel );
	payload->clear();

	std::cout << std::endl;
	return true; // send an outbound message as a result of local_work()
}

void Black::local_shutdown()
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
	Black blk("Black", "BLACK", "ipc:///tmp/feeds/broadcast", "COLOR", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//sh.world_object = argv[1];
	}
	cout << "running" << endl;
	blk.run();

	cout << "shutting down" << endl;
	return 0;
}
