//============================================================================
// Name        : Chirp.cpp
// Author      : JH
// Version     :
// Copyright   : Copyright 2019 Jimbo S. Harris. All rights reserved.
// Description : Creates a single ping on a given input channel, and
//             : listens for the response on a given output channel
//============================================================================

#include <iostream>
using namespace std;
#include "Chirp.hpp"
#include "Pixel.hpp"
#include "glm/glm.hpp"
#include <unistd.h>

void Chirp::local_setup()
{
	// Slow joiner problem
	usleep(100*1000);

	// In order to make this a "publish only" Widget,
	// The entire widget "loop" must be done here,
	// and then "running" must be set to false
	// so that loop() will be skipped but shutdown() called properly.
	Pixel pixel;
	Pixel outpx;
	msgpack::sbuffer header(0);
	msgpack::sbuffer pay(0);

	/*for( int j = 130; j < 140; j++ )
	{
		for( int i = 205; i < 215; i++ )
		{/**/
	for( int j = 0; j < camera.height; j++ )
	{
		for( int i = 0; i < camera.width; i++ )
		{/**/
			std::cout << "Pixel (" << i << "," << j << "): ";
			pixel.x = (float)i * 1.0f;
			pixel.y = (float)j * 1.0f;
			pixel.r = camera.RayThroughPoint(pixel.x, pixel.y);
			pixel.primaryRay = pixel.r;
			pixel.type = iPrimary;
			pixel.weight = 1.0f;
			pixel.depth = 0;

			printvec( "o", pixel.r.origin);
			printvec( "d", pixel.r.direction);

			header.clear();
			msgpack::pack( header, pixel );

			sendMessage(&header, &pay);
			std::cout << "\r";
			usleep(1*1000);
		}
	}
	std::cout << std::endl;

	std::cout << "finished... ";
	running = false;
}

int main()
{
	cout << "starting up" << endl;
	PixelFactory pf("Chirp", "TEST_RESPONSE", "ipc:///tmp/feeds/control", "TEST", "ipc:///tmp/feeds/control");

	cout << "running" << endl;
	pf.run();

	cout << "shutting down" << endl;
	return 0;
}
