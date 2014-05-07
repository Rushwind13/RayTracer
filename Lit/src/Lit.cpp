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
bool Lit::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object mpobj;
	unPackPart( header, &mpobj );
	mpobj.convert( &pixel );
	std::cout << "(" << pixel.x << "," << pixel.y << ") ";

	Object *obj = world.FindObject(pixel.oid);
	Light *light = world.FindLight(pixel.lid);
	assert(obj);
	assert(light);

	// Diffuse
	Color diffuse;
	diffuse = obj->color * pixel.NdotL;

	// Specular (Phong-Blinn)
	glm::vec3 vL = glm::normalize(light->position - pixel.position);
	glm::vec3 vH = vL + pixel.primaryRay.direction;
	float len = vH.length();
	float cosTheta = 0.0;
	if( len != 0.0 )
	{
		vH = vH / len;
		cosTheta = glm::dot( pixel.normal, vH );
		std::cout << "cT:" << cosTheta << "  ";
		if( cosTheta < 0.0 ) cosTheta = 0.0;
	}

	Color specular;
	float s = 1000; // TODO: shininess should come from the object definition
	specular = obj->color * pow(cosTheta, s); // TODO: different colors for specular vs diffuse?

	pixel.color = light->color * ( diffuse + specular );

	printvec("c", pixel.color);
	header->clear();
	msgpack::pack( header, pixel );
	payload->clear();

	std::cout << std::endl;
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
