//============================================================================
// Name        : Reflection.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
using namespace std;
#include "Reflection.hpp"
#include "Lighting.hpp"

void Reflection::local_setup()
{
#define DEBUG
}

// You will get here after a successful intersection with some object
// payload will contain the nearest intersection
bool Reflection::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( &pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ") ";
#endif /* DEBUG */

	// We know we have at least one hit now, so...
	pixel.gothit = true;

	Intersection i;
	msgpack::object obj2;
	unPackPart( payload, &obj2 );
	obj2.convert( &i );
	payload->clear();

	// Move necessary info out of the payload and into the header.
	// all the below tests can possibly spawn several new INTERSECT messages, which will drop the payload.

	// Reflection - if surface is reflective, send off a reflection test
	// Note: should do the reflection calc here and fill the header with any needed info (depth, weight, current oid?)
	// This will be an INTERSECT message (header only, blank payload) with the reflected ray as the "new" primary ray
	// don't overwrite the original primary ray in the header!

	// Need new ray. Origin = intersect point. Direction = reflect( primary dir, intersect normal )
	Ray rReflect;
	rReflect.origin = i.position;
	// My ReflectVector code assumes vIncident points at eye pos, this requires points at hit pos (reversed direction)
	rReflect.direction = ReflectVector( -pixel.r.direction , i.normal );
	pixel.r = rReflect;
	pixel.depth++;
	pixel.weight *= 0.5f;

	std::cout << pixel.depth << " " << pixel.weight << " ";
	// depth ++
	// weight / 2
	// type = iReflect ... or just iPrimary?

	// Refraction - if surface is refractive, send off a refraction test
	// Note: should do the refraction calc here and fill the header with any needed info (depth, weight, current oid?)
	// This will be an INTERSECT message (header only, blank payload) with the refracted ray as the "new" primary ray
	// don't overwrite the original primary ray in the header!

	header->clear();
	// Finally, calculate ambient and emissive colors and send off pixel color message
	// TODO: figure out ambient (from world) and emissive (from object) colors and prepare header and payload to send off a COLOR message

	msgpack::pack( header, pixel );
	payload->clear();
#ifdef DEBUG
	//std::cout << "(" << pixel.x << "," << pixel.y << ") ";
	//printvec("ambient", pixel.color);
	std::cout << std::endl;
#endif /* DEBUG */

	return true; // send an outbound message as a result of local_work()
}

void Reflection::local_shutdown()
{
	std::cout << "shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
	Reflection r("Reflection", "SHADE", "ipc:///tmp/feeds/broadcast", "INTERSECT", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//sh.world_object = argv[1];
	}
	cout << "running" << endl;
	r.run();

	cout << "shutting down" << endl;
	return 0;
}
