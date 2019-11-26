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
// #define DEBUG
}

// You will get here if a Shadow test registers a miss with an object between the intersection and a particular light
// so this point is lit by this light.
bool Lit::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object mpobj;
	unPackPart( header, &mpobj );
	mpobj.convert( pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ") ";
#endif /* DEBUG */

	Object *obj = world.FindObject(pixel.oid);
	Light *light = world.FindLight(pixel.lid);
	assert(obj);
	assert(light);

	// Diffuse
	Color diffuse;
	diffuse = ( obj->material.usePattern ) ? 
		obj->material.pattern.StripeAt(pixel.position) :
		obj->material.color;
	
	diffuse *= pixel.NdotL;

	float s = obj->material.shininess;
	Direction vV = glm::normalize(pixel.primaryRay.origin - pixel.position);
	Direction vL = glm::normalize(light->position - pixel.position);
	float cosTheta = 0.0;
//#define PHONG
#define PHONGBLINN
#ifdef PHONGBLINN
	// Specular (Phong-Blinn)
	Direction vH = glm::normalize(vL + vV);
	cosTheta = glm::dot( (glm::vec4)pixel.normal, (glm::vec4)vH );
	s *= 4.0f; // Blinn causes a larger spot compared to Phong; 4x change in shininess fixes it.
#endif /* PHONGBLINN */
#ifdef PHONG
	// Specular (Phong)
	Direction vR = ReflectVector( vL, pixel.normal );
	cosTheta = glm::dot( (glm::vec4)vV, (glm::vec4)vR );
#endif /* PHONG */
#ifdef DEBUG
	//std::cout << glm::length(pixel.normal) << std::endl;
	/*if( cosTheta > 0.5 )
	{
		std::cout << "cT:" << cosTheta << " ";
		printvec( "n", pixel.normal);
	}/**/
#endif /* DEBUG */
	if( cosTheta < 0.0 ) cosTheta = 0.0;
	if( cosTheta < 0.0 ) cosTheta = 0.0;

	Color specular;
	specular = light->material.color * pow(cosTheta, s); // TODO: different colors for specular vs diffuse?

#ifdef DEBUG
	if( specular.x > 0.3 )
	{
		printvec("s", specular); std::cout << std::endl;
	}
#endif /* DEBUG */
	pixel.color = (obj->material.specular * specular) +
								(obj->material.diffuse * diffuse);

	//printvec("c", pixel.color);
	header->clear();
	msgpack::pack( header, pixel );
	payload->clear();

#ifdef DEBUG
	std::cout << std::endl;
#endif
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
