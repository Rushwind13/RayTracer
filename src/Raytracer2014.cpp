//============================================================================
// Name        : Raytracer2014.cpp
// Author      : Jimbo S. Harris
// Version     :
// Copyright   : Created 3/14/2014 (Happy Pi Day)
// Description : An attempt at a message-based multi-processor Raytracing engine
//============================================================================

#include <iostream>
using namespace std;

#include "Camera.h"
#include "World.h"
#include "Raytracer2014.h"
/**
 * A brief sketch of where I am trying to go with this.
 *
 * Listeners wait for relevant messages:
 * 	- ray created
 * 	- pixel shaded
 * 	- object hit
 *
 * Listeners can do different things with the same message (OpenGL, Trace, etc)
 *
 * Anyone who needs to use the geometry should be able to use the same scenefile
 * raytracing should output a PNG
 * live rendering should output a window
 * should be able to trace a given ray through initial hit, various bounces (colored by type)
 * should be able to filter various rays (or highlight ray path via mouse hover)
 * should be able to make lights visible or not
 *
 * messaging via ZMQ
 * start with single threaded, then break out by ZMQ
 *
 */

int main(int argc, char** argv)
{
	//TODO: Commandline parsing
	//TODO: Read Scene file (JSON)
	//TODO: Setup
	//		- create world geometry
	//		- create camera
	//		- startup messaging engine
	//TODO: Run
	//		- perform traces
}

void raytracer( Camera camera, World world, Screen screen )
{
	int x, y;
	Ray ray;
	Color image[screen.h][screen.w];
	Color color;

	for( y = 0; y < screen.h; y++ )
	{
		for( x = 0; x < screen.w; x++ )
		{
			ray = RayThroughPoint( x, y, camera, camera.uvw, screen.w, screen.h );
			color = Trace( ray, world, 0, 1.0, world.refractiveindex );
			image[y][x] = color;
		}
	}
	SaveImage( image, world.filename, screen.w, screen.h );
}

Color trace( Ray ray, World world, int depth, double weight, double refractiveindex)
{
	Intersection hit = Intersect( ray, world );
	Color color = FindColor( ray, world, hit, depth, weight, refractiveindex  );
}
