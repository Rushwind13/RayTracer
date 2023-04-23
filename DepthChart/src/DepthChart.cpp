//============================================================================
// Name        : DepthChart.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Stateful Intersection results hub.
//					- listens for world objects to complete intersection tests,
//					- waits until all objects have returned results
//					- decides upon nearest intersection (if necessary)
//					- passes completed result to either Shader, Backgrounder, Shadow, or Lit widgets for processing
//============================================================================

#include <iostream>
using namespace std;
#include "DepthChart.hpp"
#include "Object.hpp"
#include "glm/glm.hpp"

void DepthChart::local_setup()
{    
//#define DEBUG
	std::cout << name << " starting up... ";
}

bool DepthChart::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ")";
#endif /* DEBUG */

	bool colorComplete = false;
	colorComplete = storeColor( pixel );

	if( colorComplete )
	{
		int64_t key = hash( pixel );
		float primary_weight = 0.67;
		float reflected_weight = 0.33;
		float refracted_weight = 0.0;
		pixel.color = (primary[key] * primary_weight) + (accumulator[key] * reflected_weight);

		// I think that as long as the weights add up to 1.0
		// and the colors are all maxed at 1.0
		// you don't need to re-max here.

		// clean up local hash tables.
		accumulator.erase(key);
		layer_count.erase(key);

#ifdef DEBUG
		std::cout << key << " done. sending: " << pixel.color.r << "  ";
#endif
		// Prepare payload for sending to next stage...
		payload->clear();
		header->clear();

		msgpack::pack( header, pixel );
#ifdef DEBUG
		//printvec("c", pixel.color);
#endif /* DEBUG *//**/
	}

#ifdef DEBUG
	std::cout << pixel.depth << std::endl;
#endif /* DEBUG *//**/
	return colorComplete; // if true, send an outbound message as a result of local_work()
}

bool DepthChart::storeColor( Pixel pixel )
{
	int64_t key = hash( pixel );
	int16_t count;
	Color curr_accumulator;
	bool testComplete = false;
	Object *obj = world.FindObject( pixel.oid );

	// on a BKG (miss), that sets the number of layers
	if( pixel.gothit == false )
	{
		// There won't be any more layers after this one.
		// you know that this is the only one you're getting (at least for this depth).
		maxlayers[key] = pixel.depth + 1;

#ifdef DEBUG
		// std::cout << " BKG hit at layer " << pixel.depth << ". set layers to " << maxlayers[key] << " ";
#endif
	}
	else if( obj && obj->material.reflective < epsilon )
	{
		maxlayers[key] = pixel.depth + 1;

#ifdef DEBUG
		std::cout << " non-reflective hit at layer " << pixel.depth << ". set layers to " << maxlayers[key] << " ";
#endif
	}
	// ...otherwise, first time in, expect to get hits on each layer
	else if( layer_count.find(key) == layer_count.end() )
	{
		maxlayers[key] = world.maxdepth + 1;
#ifdef DEBUG
		std::cout << " new one. set layers to " << maxlayers[key] << " ";
#endif
	}

	if( pixel.depth == 0 )
	{
		primary[key] =  pixel.color;
	}
	else
	{
		curr_accumulator = accumulator[key];
		curr_accumulator += pixel.color * pixel.weight;
		if( curr_accumulator.r > 1.0 ) curr_accumulator.r = 1.0;
		if( curr_accumulator.g > 1.0 ) curr_accumulator.g = 1.0;
		if( curr_accumulator.b > 1.0 ) curr_accumulator.b = 1.0;

		accumulator[key] = curr_accumulator;
	}
	layer_count[key]++;

	return (layer_count[key] == maxlayers[key]);
}

void DepthChart::local_shutdown()
{
	std::cout << "ColorResults shutting down... ";
	layer_count.clear();
	accumulator.clear();
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 6 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	DepthChart dc(argv[1], argv[2], argv[3], argv[4], argv[5]);
	cout << "running" << endl;
	dc.run();

	cout << "shutting down" << endl;
	return 0;
}
