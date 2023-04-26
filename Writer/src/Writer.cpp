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
//#define DEBUG
#define UGLY
#ifdef UGLY
    Color ugly(1.0, 0.0, 0.0);
    for( int i=0; i < pixel_count; i++ )
    {
        image[i] = ugly;
    }
#endif /* UGLY */
	std::cout << pixel_count << " pixels.";
}

bool Writer::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ")";
	printvec("c", pixel.color);
#endif /* DEBUG */

    if( pixel.type == iInvalid )
    {
        running = false;
        std::cout << "finishing up" << std::endl;
        return false;
    }

	bool fileComplete = false;
	fileComplete = storePixel( pixel );
#ifdef DEBUG
	std::cout << " " << pixel_count;
#endif /* DEBUG */

	if( fileComplete )
	{
#ifdef DEBUG
		std::cout << std::endl;
#endif /* DEBUG */
		// You got a full set of responses for this file, so
		// time to write a PNG...
		SaveImage();

		// and get ready to run again
		//pixel_count = 100;
		pixel_count = camera.width * camera.height;
	}
#ifdef DEBUG
	std::cout << std::endl;
#endif /* DEBUG */
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
    else if( pixel_count % 1000 == 0)
    {
        std::cout << pixel_count << std::endl;
        SaveImage(); // we keep losing pixels...
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

void Writer::local_shutdown()
{
	std::cout << "Writer shutting down... ";
    SaveImage();
	response_count.clear();
	accumulator.clear();
	delete image;
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 5 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Writer wr(argv[1], argv[2], argv[3], "", "");
    strcpy(wr.world.filename, argv[4]);

	cout << "running" << endl;
	wr.run();

	cout << "shutting down" << endl;
	return 0;
}
