//============================================================================
// Name        : Lit.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
using namespace std;
#include "Lit.hpp"
#include "Lighting.hpp"

void Lit::local_setup()
{

}

// You will get here if a Shadow test registers a miss with an object between the intersection and a particular light
// so this point is lit by this light.
bool Lit::local_work(byte_vector *header, byte_vector *payload)
{
	std::cout << "doing work... ";
	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	Object *obj = world.FindObject(pixel.oid);
	Light *light = world.FindLight(pixel.lid);

	// Diffuse
	Color diffuse;
	diffuse = obj->color * pixel.NdotL;

	// Specular (Phong-Blinn)
	vec3 vL = normalize(light->position - pixel.position);
	vec3 vH = vL + pixel.primaryRay.direction;
	float len = vH.length();
	float cosTheta = 0.0;
	if( len != 0.0 )
	{
		vH = vH / len;
		cosTheta = dot( pixel.normal, vH );
		if( cosTheta < 0.0 ) cosTheta = 0.0;
	}

	Color specular;
	float s = 10; // TODO: shininess should come from the object definition
	specular = obj->color * pow(cosTheta, s); // TODO: different colors for specular vs diffuse?

	pixel.color = light->color * ( diffuse + specular );

	encodeBuffer( header, (void *)&pixel, sizeof(Pixel));
	payload->clear();

	return true; // send an outbound message as a result of local_work()
}

void Lit::local_shutdown()
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
	Lit bak("Lit", "LIT", "ipc:///tmp/feeds/broadcast", "COLOR", "ipc:///tmp/feeds/control");

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
