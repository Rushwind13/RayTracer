//============================================================================
// Name        : PixelFactory.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Creates all the primary rays for the raytracing engine, one ray per pixel.
//============================================================================

#include <iostream>
using namespace std;
#include "PixelFactory.hpp"
#include "Pixel.hpp"
#include "glm/glm.hpp"
#include <unistd.h>

void PixelFactory::local_setup()
{
	// Slow joiner problem
	usleep(100*1000);

	camera.setup();

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
			std::cout << "Pixel (" << std::setw(3) << i << "," << std::setw(3) << j << "): ";
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
	PixelFactory pf("PixelFactory", "", "", "INTERSECT", "ipc:///tmp/feeds/control");

	cout << "running" << endl;
	pf.run();

	cout << "shutting down" << endl;
	return (int)0;
}
