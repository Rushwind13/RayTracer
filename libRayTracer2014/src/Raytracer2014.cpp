//============================================================================
// Name        : Raytracer2014.cpp
// Author      : Jimbo S. Harris
// Version     :
// Copyright   : Created 3/14/2014 (Happy Pi Day)
// Description : An attempt at a message-based multi-processor Raytracing engine
//============================================================================

#include <iostream>
//using namespace std;

#include "Camera.hpp"
#include "World.hpp"
#include "Lighting.hpp"
#include "Raytracer2014.hpp"
#include <pngwriter.h>

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
	//FreeImage_Initialise();

	//		- create world geometry
	World world;
	strcpy( world.filename, "./test.png" );

	Position center(-2.5,0.0,-5.0,1.0);
	float radius=2.0;
	Sphere sphere(center, radius);
	sphere.oid = 1;
	sphere.name = "sphere1";
	world.objects.push_back(&sphere);
	Position center2(2.5,0.0,-10.0,1.0);
	float radius2=4.0;
	Sphere sphere2(center2, radius2);
	sphere2.color = Color(0.0,0.0,1.0);
	sphere2.oid = 2;
	sphere.name = "sphere2";
	world.objects.push_back(&sphere2);

	Position lpos0(-2.5, 0.0, -1.0,1.0);
	Light light0(lpos0);
	light0.color = Color(1.0,1.0,1.0);
	light0.oid = 0;
	light0.name = "light0";
	world.lights.push_back(&light0);

	//		- create camera
	Position eye(0.0,0.0,0.0,1.0);
	Position lookAt(0.0,0.0,-1.0,1.0);
	glm::vec3 up(0.0,1.0,0.0);
	float fovy = 90.0;
	float width = 900.0;
	float height = 600.0;
	Camera camera(eye, lookAt, up, fovy, width, height);
	//		- startup messaging engine
	//TODO: Run
	//		- perform traces
	raytracer( camera, world );
}

void raytracer( Camera camera, World world )
{
	int x, y;
	//Color *image = new Color[ camera.height * camera.width ];
	std::list<Color> image;

	for( y = 1; y < camera.height; y++ )
	{
		for( x = 1; x < camera.width; x++ )
		{
			// TODO: to implement messaging, create a Pixel instance here, send to RayThroughPoint (via message)... and that's it, I think.
			// RayThroughPoint will pass it along to Trace, which will pass it along to...
			Color color;
			Ray ray = camera.RayThroughPoint( x, y );
			color = Trace( ray, world, 0, 1.0, world.refractiveindex );
			//image[y * camera.width + x] = color;
			image.push_back(color);
			//std::cout << color.x;// << color.y << color.z;
		}
		//std::cout << std::endl;
	}
	std::cout  << std::endl << "finishing up" << std::endl;
	SaveImage( image, world, camera );
}

Color Trace( Ray ray, World world, int depth, float weight, float refractiveindex)
{
	Intersection hit = world.Intersect( ray );

	Color color = FindColor( ray, world, hit, depth, weight, refractiveindex  );

	return color;
}

Color FindColor( Ray ray, World world, Intersection hit, int depth, float weight, float refractiveindex )
{
	Color pixel(0.2,0,0.23);
	if( !hit.gothit ) return pixel;

	//TODO: For now, just return the object's color, but this is where the lighting magic happens...

	// foreach light
	// call lighting.Shadow(), boolean. true = shadowed, no contrib from this light to this point.
	// false = more lighting magic... (diffuse, specular, reflect, refract)
	Lighting lighting;

	for( std::list<Light *>::iterator it = world.lights.begin(); it != world.lights.end(); it++ )
	{
		// actually, first create the vector to the light, and take N.L to see if light is below surface
		// shouldn't it sometimes be below the surface during refraction? hmm.
		Light *light = *it;

		glm::vec3 vL;
		float light_dist;
		lighting.vL(*light, hit, vL, light_dist );

		float NdotL = glm::dot( hit.normal, vL );

		if( NdotL < 0 )
		{
			// light comes from below surface
			pixel =  Color(0.0,0.0,0.0);
			continue;
		}

		// Light is above surface. Anything between this point and the light?
		Ray rShadow;
		lighting.Shadow( vL, hit, &rShadow );
		Intersection shadow = world.Intersect( rShadow, light_dist );
		if( shadow.gothit )
		{
			/*if( shadow.object == hit.object )
			{
				std::cout << "self hit ";
			}
			else if( shadow.object != hit.object )
			{
				int foo =1;
				//std::cout << hit.object->oid << "x" << shadow.object->oid << " ";

			}/**/
			pixel = Color(0.1,0.1,0.1);
			continue;
		}

		// point is unshadowed, and thus is lit by this light
		//printvec( "color", hit.object->color * NdotL );
		return Color(0.1,0.1,0.1);//hit.object->color * NdotL;
	}

	// Show that we actually hit something, shadowy...
	return pixel;
}
void SaveImage( std::list<Color> image, const World world, const Camera camera )
{
	pngwriter png(camera.width,camera.height,0,world.filename);
	std::list<Color>::iterator it = image.begin();
	for( int y = 1; y < camera.height; y++ )
	{
		for(int x = 1; x < camera.width; x++ )
		{
			png.plot(x,y, (*it).x, (*it).y, (*it).z);
			it++;
		}
	}
	if(  it != image.end() )
	{
		std::cout << "Seems like there's too much data in the image array.." << std::endl;
	}
	png.close();
}
/*
void SaveImage( std::list<Color> image, const World world, const Camera camera )
{
	unsigned char bytes[camera.height * camera.width * 3];
	memset( bytes, 0, sizeof(bytes));

	unsigned char *current_byte = bytes;


	FILE *fp = fopen( world.filename, "w" );
	if( !fp ) return;

	for(std::list<Color>::iterator it = image.begin(); it != image.end(); it++)
	{
		*current_byte++ = (*it).x;
		*current_byte++ = (*it).y;
		*current_byte++ = (*it).z;
		fputc((*it).x, fp);
		fputc((*it).y, fp);
		fputc((*it).z, fp);

		//fprintf(fp, "%c", (*it).x);
	}

	fclose(fp);

	/*FIBITMAP* img = FreeImage_ConvertFromRawBits(bytes, camera.width, camera.height, camera.width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	std::cout << "Saving screenshot: " << world.filename << "\n";
	FreeImage_Save(FIF_PNG, img, world.filename, 0);
	// world.filename
	// camera.width
	// camera.height
}/**/
