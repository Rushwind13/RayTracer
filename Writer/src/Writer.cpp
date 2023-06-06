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
    Color ugly(0.5, 0.5, 0.5);
    for( int i=0; i < pixel_count; i++ )
    {
        image[i] = ugly;
    }
#endif /* UGLY */
	std::cout << pixel_count << " pixels.";
    pixel_count=0;
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
        std::cout << "Writer got EOF; finishing up" << std::endl;
        SaveImage();
        return false;
    }

	int curr_pixel = (int)((pixel.y*camera.width) + pixel.x);
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    // we have an incomplete pixel, so pixel.color is not set (or is incorrect)
    // use cases:
    // - pixel is a MISS (gothit=F), p.oid unset, i.oid unset -- choose annoying color (or 0,0,0)
    // - pixel is a HIT (gothit=T), i.oid set, IW (obox1.txt) will not have p.oid
    Intersection i;
    msgpack::object obj2;
    unPackPart( payload, &obj2 );
    obj2.convert( i );

    // check for intersection oid first; it will be an interesting piece of (new!) data (even if pixel.oid is set)
    // pixel.oid will be set on anything that is downstream of Shader (including anything with depth>0)
    Object *world_object;
    Color diffuse(0.0,1.0,1.0);

    if(i.oid != -1 )
    {
        world_object = world.FindObject(i.oid);
        diffuse = world_object->ColorAt(i.position);
    }
    else if(pixel.oid != -1 )
    {
        world_object = world.FindObject(pixel.oid);
        diffuse = world_object->ColorAt(pixel.position);
    }

    image[curr_pixel] = diffuse;
#else
	image[curr_pixel] = pixel.color;
#endif /* BRUTE_FORCE */

    AutosaveImage();
#ifdef DEBUG
	std::cout << " " << pixel_count << std::endl;
#endif /* DEBUG */
	return false; // no more messages; we're done.
}

void Writer::AutosaveImage()
{
	pixel_count++;
    if( pixel_count % 1000 == 0)
    {
        std::cout << pixel_count << std::endl;
        SaveImage(); // we keep losing pixels...
    }
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
