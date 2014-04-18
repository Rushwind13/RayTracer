//============================================================================
// Name        : Shader.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
using namespace std;
#include "Shader.hpp"
#include "Pixel.hpp"
#include "Lighting.hpp"

void Shader::local_setup()
{

}

// You will get here after a successful intersection with some object
// payload will contain the nearest intersection
bool Shader::local_work(byte_vector *header, byte_vector *payload)
{
	std::cout << "doing work... ";
	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	Intersection i;
	memcpy( payload, &i, sizeof(Intersection) );

	// Move necessary info out of the payload and into the header.
	// all the below tests can possibly spawn several new INTERSECT messages, which will drop the payload.

	// Reflection - if surface is reflective, send off a reflection test
	// Note: should do the reflection calc here and fill the header with any needed info (depth, weight, current oid?)
	// This will be an INTERSECT message (header only, blank payload) with the reflected ray as the "new" primary ray
	// don't overwrite the original primary ray in the header!

	// Refraction - if surface is refractive, send off a refraction test
	// Note: should do the refraction calc here and fill the header with any needed info (depth, weight, current oid?)
	// This will be an INTERSECT message (header only, blank payload) with the refracted ray as the "new" primary ray
	// don't overwrite the original primary ray in the header!

	Lighting lighting;
	// foreach light
	for( std::list<Light *>::iterator it = world.lights.begin(); it != world.lights.end(); it++ )
	{
		// actually, first create the vector to the light, and take N.L to see if light is below surface
		// shouldn't it sometimes be below the surface during refraction? hmm.
		Light *light = *it;

		vec3 vL;
		float light_dist;
		lighting.vL(*light, i, vL, light_dist );

		float NdotL = dot( i.normal, vL );

		//   N.L < 0 = send off background color message
		if( NdotL < 0 )
		{
			// light comes from below surface
			// TODO: Send off a BKG message to set this to background color
			continue;
		}

		// Light is above surface. Anything between this point and the light?
		Ray rShadow;
		lighting.Shadow( vL, i, &rShadow );

		// TODO: send off an INTERSECT message with rShadow as the testing ray and iShadow as the test type
		// Note, need to save off NdotL, any info needed (like vL, this oid, etc that will be needed by the specular and diffuse calcs)
		// need to save these in the *header*, as INTERSECT does not take a payload.
	}

	// Finally, calculate ambient and emissive colors and send off pixel color message
	// TODO: figure out ambient (from world) and emissive (from object) colors and prepare header and payload to send off a COLOR message

	return true; // send an outbound message as a result of local_work()
}

void Shader::local_shutdown()
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
	Shader sh("Shader", "SHADE", "ipc:///tmp/feeds/broadcast", "COLOR", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//sh.world_object = argv[1];
	}
	cout << "running" << endl;
	sh.run();

	cout << "shutting down" << endl;
	return 0;
}
