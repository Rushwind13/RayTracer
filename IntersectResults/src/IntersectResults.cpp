//============================================================================
// Name        : IntersectResults.cpp
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
#include "IntersectResults.hpp"
#include "Object.hpp"
#include "Pixel.hpp"
#include "glm/glm.hpp"

void IntersectResults::local_setup()
{
	std::cout << "IntersectResults starting up... ";
}

bool IntersectResults::local_work(byte_vector *header, byte_vector *payload)
{
	std::cout << "doing work... ";
	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	Intersection i;
	memcpy( payload, &i, sizeof(Intersection) );

	// Shadow tests only require one hit, so don't bother calculating the nearest one.
	// otherwise, use a hash table to keep the nearest hit and the count of world objects that have reported so far.
	// Until all objects have reported in (for this test), don't send out a message.
	// once all objects have reported in, send the nearest hit out.

	//payload->insert(payload->begin(), i, i + sizeof (Intersection) );

	return true; // send an outbound message as a result of local_work()
}

void IntersectResults::local_shutdown()
{
	std::cout << "IntersectResults shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
	IntersectResults iw("IntersectWith", "RESULT", "ipc:///tmp/feeds/broadcast", "SHADE", "ipc:///tmp/feeds/control");

	if( argc > 1 )
	{
		int foo = 1;
		//iw.world_object = argv[1];
	}
	cout << "running" << endl;
	iw.run();

	cout << "shutting down" << endl;
	return 0;
}
