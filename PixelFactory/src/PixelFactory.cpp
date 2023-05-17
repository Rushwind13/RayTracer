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
#include "glm/glm.hpp"
#include <unistd.h>
//#define DEBUG

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
#ifdef DEBUG
			std::cout << "Pixel (" << i << "," << j << "): ";
#endif /* DEBUG */
			pixel.x = (float)i * 1.0f;
			pixel.y = (float)j * 1.0f;
			pixel.r = camera.RayThroughPoint(pixel.x, pixel.y);
			pixel.primaryRay = pixel.r;
			pixel.type = iPrimary;
			pixel.weight = 1.0f;
			pixel.depth = 0;
            pixel.NdotL = 0.0f;
            pixel.distance = 1e9;
#ifdef DEBUG
			printvec( "o", pixel.r.origin);
			printvec( "d", pixel.r.direction);
#endif /* DEBUG */

            pixels.push_back(pixel);
			// header.clear();
			// msgpack::pack( header, pixel );
            //
			// sendMessage(&header, &pay);
#ifdef DEBUG
			std::cout << "\r";
#endif /* DEBUG */
			usleep(1*1000);
		}
	}
	std::cout << std::endl;

    // write pixels out to file
    std::ofstream out("output.txt");
    std::vector<Pixel>::iterator ptr;
    for( ptr = pixels.begin(); ptr < pixels.end(); ptr++)
    {
        PrintPixel(out, *ptr);
    }
    out.close();
#undef WANT_EOF
#ifdef WANT_EOF
    // send "EOF" pixel
    pixel.x = -1.0f;
    pixel.y = -1.0f;
    pixel.type = iInvalid;
    pixel.weight = 0.0f;
	header.clear();
	msgpack::pack( header, pixel );

	sendMessage(&header, &pay);
#endif /* WANT_EOF */

	std::cout << "finished... ";
	running = false;
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 4 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	PixelFactory pf(argv[1], "", "", argv[2], argv[3]);

	cout << "running" << endl;
	pf.run();

	cout << "shutting down" << endl;
	return 0;
}
