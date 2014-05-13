//============================================================================
// Name        : Writer.cpp
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
#include "Writer.hpp"
#include "Object.hpp"
#include "glm/glm.hpp"
#include <pngwriter.h>

void Writer::local_setup()
{
	std::cout << name << " starting up... ";
	camera.setup();
	//pixel_count = 100;
	pixel_count = camera.width * camera.height;
	image = new Color[pixel_count];
	std::cout << pixel_count << " pixels.";
}

bool Writer::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( &pixel );
	std::cout << "(" << pixel.x << "," << pixel.y << ")";
	printvec("c", pixel.color);

	bool fileComplete = false;
	fileComplete = storePixel( pixel );
	std::cout << " " << pixel_count;

	if( fileComplete )
	{
		std::cout << std::endl;
		// You got a full set of responses for this file, so
		// time to write a PNG...
		SaveImage();

		// and get ready to run again
		//pixel_count = 100;
		pixel_count = camera.width * camera.height;
	}

	std::cout << std::endl;
	return false; // no more messages; we're done.
}

bool Writer::storePixel( Pixel pixel )
{
	bool fileComplete = false;

	pixel_count--;
	if( pixel_count == 0 )
	{
		fileComplete = true;
	}

	int curr_pixel = (int)((pixel.y*camera.width) + pixel.x);
	image[curr_pixel] = pixel.color;

	return fileComplete;
}

void Writer::SaveImage()
{
	std::cout << "Writing file " << world.filename << "...";
	pngwriter png(camera.width,camera.height,0,world.filename);
	Color *curr_pixel = image;
	for( int y = 1; y <= camera.height; y++ )
	{
		for(int x = 1; x <= camera.width; x++ )
		{
			png.plot(x,y, curr_pixel->x, curr_pixel->y, curr_pixel->z);
			curr_pixel++;
		}
	}
	png.close();
	std::cout << "done." << std::endl;
}

/*void Writer::local_send( msgpack::sbuffer *header, msgpack::sbuffer *payload )
{
	// Depending on whether it is a hit or a miss, and depending upon the test type,
	// this function will publish the intersection to one of 4 places.
	// 			HIT				MISS
	// SHADOW	BLACK			LIT
	// other	SHADE			BKG

	Pixel pixel;
	memcpy( header, &pixel, sizeof(Pixel) );

	Intersection i;
	memcpy( payload, &i, sizeof(Intersection) );

	char pub[6] = "";

	if( pixel.type == iShadow )
	{
		// Shadow rays get "Black" when they hit something, or "Lit" when they miss
		strcpy(pub, (i.gothit && (i.distance < pixel.distance)) ? "BLACK":"LIT");
	}
	else
	{
		// All other ray types (Primary, Reflection, Refraction)
		// get "Shade" when they hit something, or "Background" when they miss
		strcpy( pub, i.gothit ? "SHADE":"BKG");
	}

	sendMessage( header, payload, pub );
}/**/

void Writer::local_shutdown()
{
	std::cout << "IntersectResults shutting down... ";
	response_count.clear();
	accumulator.clear();
	delete image;
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
	Writer wr("Writer", "PNG", "ipc:///tmp/feeds/broadcast", "", "");

	if( argc > 1 )
	{
		strcpy(wr.world.filename, argv[1]);
	}
	cout << "running" << endl;
	wr.run();

	cout << "shutting down" << endl;
	return 0;
}
