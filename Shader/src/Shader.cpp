//============================================================================
// Name        : Shader.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
#include <unistd.h>
using namespace std;
#include "Shader.hpp"
#include "Lighting.hpp"

void Shader::local_setup()
{
//#define DEBUG
    // std::cout << "creating sockets for alternate pub paths...";
    // black_publisher = new zmq::socket_t( *context, ZMQ_PUB );
    // black_publisher->connect( "tcp://127.0.0.1:1312" ); // BLACK
    //
    // intersect_publisher = new zmq::socket_t( *context, ZMQ_PUB );
    // intersect_publisher->connect( "tcp://127.0.0.1:1313" ); // INTERSECT
    // std::cout << "done." << std::endl;

    pixel_count=0;
}

// You will get here after a successful intersection with some object
// payload will contain the nearest intersection
bool Shader::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
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

        sendMessage(header, payload, "Black");

        header->clear();
        payload->clear();
        msgpack::pack( header, pixel );
        msgpack::pack( payload, i );
        sendMessage(header, payload, "IntersectWith");

        header->clear();
        payload->clear();
        msgpack::pack( header, pixel );
        msgpack::pack( payload, i );
        sendMessage(header, payload, "Shader");

        std::cout << "sent." << std::endl;
        pixel_count = 0;
        usleep(100*1000); // slow re-joiner problem?
        return false;
    }

	// We know we have at least one hit now, so...
	pixel.gothit = true;

	msgpack::object obj2;
	unPackPart( payload, &obj2 );
	obj2.convert( i );

    pixel_count++;
    std::cout << "(" << i.oid << "-"<< pixel.y << ")" << "\r";

	payload->clear();
	Intersection i2;
    msgpack::pack( payload, i2 );

	// Move necessary info out of the payload and into the header.
	// all the below tests can possibly spawn several new INTERSECT messages, which will drop the payload.

	Color ambient(0.02,0.02,0.02);// TODO: this comes from the world
	Color emissive(0.0,0.0,0.0); // TODO: this comes from the object

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

		Direction vL;
		float light_dist;
		lighting.vL(*light, i, vL, light_dist );

		float NdotL = glm::dot( (glm::vec4)i.normal, (glm::vec4)vL );

		//   N.L < 0 = send off background color message
		if( NdotL < 0.0) //(ambient.r + emissive.r) ) // TODO: <-- this has to be a bug
		{
#ifdef DEBUG
			std::cout << "(" << pixel.x << "," << pixel.y << ")" << " N.L < 0 for lid: " << light->oid << std::endl;
#endif /* DEBUG */
			// light comes from below surface
			// TODO: Send off a BKG message to set this to background color
			sendMessage(header, payload, "Black");
			//sendMessage(header, payload, "BKG");
			continue;
		}

		// Light is above surface. Anything between this point and the light?
		Ray rShadow;
		lighting.Shadow( vL, i, &rShadow );
#ifdef NOTDEFINED
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
#endif /* 0 */
		pixel.r = rShadow;
		pixel.distance = light_dist;
		pixel.NdotL = NdotL;
		pixel.lid = light->oid;
		prepareShadowTest( &pixel, i );

		header->clear();

		msgpack::pack( header, pixel );

		sendMessage( header, payload, "IntersectWith");
#ifdef DEBUG
		Pixel px2;

		msgpack::object obj3;
		unPackPart( header, &obj3 );
		obj3.convert( px2 );

		//std::cout << " shadow test for oid: " << px2.oid << " lid: " << px2.lid << " " << px2.type << " ";
		//printvec( "rS.o", px2.r.origin );
		//printvec( "rS.d", px2.r.direction );
		//std::cout << std::endl;
#endif /* DEBUG */
	}

	header->clear();
	// Finally, calculate ambient and emissive colors and send off pixel color message
	// TODO: figure out ambient (from world) and emissive (from object) colors and prepare header and payload to send off a COLOR message
	pixel.type = iPrimary;
	pixel.color = ambient + emissive;

	msgpack::pack( header, pixel );
	payload->clear();
	msgpack::pack( payload, i2 );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ") ";
	printvec("ambient", pixel.color);
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
	//pixel->distance = i.distance;
}

void Shader::local_shutdown()
{
	std::cout << "shutting down... ";

    if( black_publisher != NULL )
	{
		black_publisher->close();
		black_publisher = NULL;
	}
	if( intersect_publisher != NULL )
	{
		intersect_publisher->close();
		intersect_publisher = NULL;
	}
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 6 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
    Shader sh(argv[1], argv[2], argv[3], argv[4], argv[5]);
	cout << "running" << endl;
	sh.run();

	cout << "shutting down" << endl;
	return 0;
}
