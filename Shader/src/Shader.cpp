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
#include "Lighting.hpp"

void Shader::local_setup()
{

}

// You will get here after a successful intersection with some object
// payload will contain the nearest intersection
bool Shader::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( &pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ")" << pixel.type << " ";
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

		glm::vec3 vL;
		float light_dist;
		lighting.vL(*light, i, vL, light_dist );

		float NdotL = glm::dot( i.normal, vL );

		//   N.L < 0 = send off background color message
		if( NdotL < 0 )
		{
#ifdef DEBUG
			std::cout << " N.L < 0 for lid: " << light->oid;
#endif /* DEBUG */
			// light comes from below surface
			// TODO: Send off a BKG message to set this to background color
			sendMessage(header, payload, "BLACK");
			continue;
		}

		// Light is above surface. Anything between this point and the light?
		Ray rShadow;
		lighting.Shadow( vL, i, &rShadow );
		Pixel pShadow;

		pShadow.x = pixel.x;
		pShadow.y = pixel.y;
		pShadow.primaryRay = pixel.primaryRay;
		pShadow.color = pixel.color;
		pShadow.gothit = pixel.gothit;
		pShadow.depth = pixel.depth;

		// TODO: send off an INTERSECT message with rShadow as the testing ray and iShadow as the test type
		// Note, need to save off NdotL, any info needed (like vL, this oid, etc that will be needed by the specular and diffuse calcs)
		// need to save these in the *header*, as INTERSECT does not take a payload.
		pShadow.r = rShadow;
		pShadow.NdotL = NdotL;
		pShadow.lid = light->oid;

		// this will get called a lot -- could probably move it outside of the for loop (especially if needed for the reflection/refraction stuff)
		// TODO: Should this get done by IntersectionResults?
		prepareShadowTest( &pShadow, i );

		header->clear();

		msgpack::pack( header, pShadow );

		sendMessage( header, payload, "INTERSECT" );

		Pixel px2;

		msgpack::object obj3;
		unPackPart( header, &obj3 );
		obj3.convert( &px2 );

		//std::cout << " shadow test for oid: " << px2.oid << " lid: " << px2.lid << " " << px2.type << " ";
		//printvec( "rS.o", px2.r.origin );
		//printvec( "rS.d", px2.r.direction );
		//std::cout << std::endl;
	}

	header->clear();
	// Finally, calculate ambient and emissive colors and send off pixel color message
	// TODO: figure out ambient (from world) and emissive (from object) colors and prepare header and payload to send off a COLOR message
	pixel.type = iPrimary;
	Color ambient(0.1,0.1,0.1);
	Color emissive(0.0,0.0,0.0); // TODO: this comes from the object
	pixel.color = ambient + emissive;

	msgpack::pack( header, pixel );
	payload->clear();
#ifdef DEBUG
	//std::cout << "(" << pixel.x << "," << pixel.y << ") ";
	//printvec("ambient", pixel.color);
	std::cout << std::endl;
#endif /* DEBUG */

	return true; // send an outbound message as a result of local_work()
}

// Copy the info out of the Intersection to pass along to further tests
void Shader::prepareShadowTest( Pixel *pixel, const Intersection i )
{
	pixel->type = iShadow;
	pixel->oid = i.oid;
	pixel->normal = i.normal;
	pixel->position = i.position;
	pixel->distance = i.distance;
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
