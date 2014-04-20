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

void PixelFactory::local_setup()
{
	camera.setup();

	// In order to make this a "publish only" Widget,
	// The entire widget "loop" must be done here,
	// and then "running" must be set to false
	// so that loop() will be skipped but shutdown() called properly.
	Pixel pixel;
	unsigned char *buffer;
	byte_vector hdr;
	byte_vector pay;
	pay.clear();
	for( int j = 0; j < camera.height; j++ )
	{
		for( int i = 0; i < camera.width; i++ )
		{
			pixel.x = (float)i;
			pixel.y = (float)j;
			pixel.r = camera.RayThroughPoint(pixel.x, pixel.y);
			pixel.primaryRay = pixel.r;
			pixel.type = iPrimary;

			buffer = (unsigned char *)malloc( sizeof( Pixel ) + 1 );
			memset( buffer, 0, sizeof( Pixel ) + 1 );
			memcpy(&pixel, buffer, sizeof( Pixel ));
			hdr.insert( hdr.begin(), buffer, buffer + sizeof( Pixel ) + 1 );
			sendMessage(&hdr, &pay);
			free(buffer);
		}
	}

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
	return 0;
}
