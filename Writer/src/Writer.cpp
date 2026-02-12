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
#include <unordered_map>
#include <map>

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
        // Report color diversity and per-object contributions
        std::cout << "[STATS] distinct_colors: " << color_hist.size() << std::endl;
        // Print per-object counts sorted by count desc
        std::map<long long, std::vector<int>, std::greater<long long>> by_count;
        for (auto &kv : object_hist) { by_count[kv.second].push_back(kv.first); }
        for (auto &bucket : by_count) {
            long long cnt = bucket.first;
            for (int oid : bucket.second) {
                Object *obj = world.FindObject(oid);
                const char* name = obj ? obj->name.c_str() : "<none>";
                std::cout << "[STATS] object: " << name << " (oid=" << oid << ") pixels=" << cnt << std::endl;
            }
        }
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
    try {
        msgpack::object obj2;
        unPackPart( payload, &obj2 );
        obj2.convert( i );
    } catch (const std::exception&) {
        // If payload is missing or malformed, treat as no intersection
        i.gothit = false;
        i.oid = -1;
    }

    // check for intersection oid first; it will be an interesting piece of (new!) data (even if pixel.oid is set)
    // pixel.oid will be set on anything that is downstream of Shader (including anything with depth>0)
    Object *world_object;
    Color diffuse(0.0,1.0,1.0);

    int used_oid = -1;
    if(i.oid != -1 )
    {
        world_object = world.FindObject(i.oid);
        if (world_object) diffuse = world_object->ColorAt(i.position);
        used_oid = i.oid;
    }
    else if(pixel.oid != -1 )
    {
        world_object = world.FindObject(pixel.oid);
        if (world_object) diffuse = world_object->ColorAt(pixel.position);
        used_oid = pixel.oid;
    }

    image[curr_pixel] = diffuse;
    // Update histograms
    if (used_oid != -1) { object_hist[used_oid]++; }
    // Quantize color to 8-bit per channel and pack into 24-bit code
    auto q = [](float v)->unsigned int {
        if (v < 0.f) v = 0.f; if (v > 1.f) v = 1.f; return (unsigned int)(v * 255.0f + 0.5f);
    };
    unsigned int r = q(diffuse.x), g = q(diffuse.y), b = q(diffuse.z);
    uint32_t code = (r << 16) | (g << 8) | b;
    color_hist[code]++;
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

    // Allow binding the subscriber (PNG bus) in isolated runs so Feeder can connect directly
    const char* bind_sub = std::getenv("WRITER_BIND_SUB");
    if (bind_sub && *bind_sub && *bind_sub != '0') {
        wr.forceBindSubscriber();
    }

	cout << "running" << endl;
	wr.run();

	cout << "shutting down" << endl;
	return 0;
}
