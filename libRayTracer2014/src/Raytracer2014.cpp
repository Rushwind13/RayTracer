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

	glm::vec3 center(-2.5,0.0,-5.0);
	float radius=2.0;
	Sphere sphere(center, radius);
	sphere.oid = 1;
	sphere.name = "sphere1";
	world.objects.push_back(&sphere);
	glm::vec3 center2(2.5,0.0,-10.0);
	float radius2=4.0;
	Sphere sphere2(center2, radius2);
	sphere2.color = Color(0.0,0.0,1.0);
	sphere2.oid = 2;
	sphere.name = "sphere2";
	world.objects.push_back(&sphere2);

	Position lpos0(-2.5, 0.0, -1.0);
	Light light0(lpos0);
	light0.color = Color(1.0,1.0,1.0);
	light0.oid = 0;
	light0.name = "light0";
	world.lights.push_back(&light0);

	//		- create camera
	glm::vec3 eye(0.0,0.0,0.0);
	glm::vec3 lookAt(0.0,0.0,-1.0);
	glm::vec3 up(0.0,1.0,0.0);
	float fovy = 90.0;
	float width = 900.0;
	float height = 600.0;
	Camera camera(eye, lookAt, up, fovy, width, height);
	//		- startup messaging engine
	//TODO: Run
	//		- perform traces
	raytracer( camera, world );

	/**
	 *
#####
#
#  MAIN ROUTINES
#
#####
import sys
import getopt

def process(arg):
	return

class Usage(Exception):
    def __init__(self, msg):
        self.msg = msg

prefs = {'verbose':0, 'rotate':False}
def main(argv=None):
	if argv is None:
		argv = sys.argv
	try:
		try:
			opts, args = getopt.getopt(argv[1:], "hvr", ["help", "verbose", "rotation"])
		except getopt.error, msg:
			raise Usage(msg)

		# process options
		for o, a in opts:
			if o in ("-h", "--help"):
				print """ bridge.py <file1> <file2> ... <filen> """
				return 0
			elif o in ("-v", "--verbose"):
				prefs['verbose'] += 1
			elif o in ("-r", "--rotation"):
				prefs['rotate'] = True
		# process arguments
		for arg in args:
			scenefile = arg
			process(arg) # process() is defined elsewhere
	except Usage, err:
		print >>sys.stderr, err.msg
		print >>sys.stderr, "for help use --help"
		return 2

	#scenefile = argv[1]
	if not scenefile: scenefile = "scenes/jimbo_scene1.txt"
 	# parse input file
	print('reading input file\n')
 	metadata = readjson.parse( scenefile )
 	print('done\n')
 	camera = create_camera(metadata)
 	screen = create_screen(metadata)
 	scene = create_scene(metadata)
 	if prefs['rotate']:
		radius = la.norm(camera['eye'])#*2.
		theta = 0.
		while theta < 360.:
			scene['filename'] = 'rotation_'+str(int(theta))+'.png'
			th = deg2rad(theta)
			camera['eye'] = array([radius * cos(th), radius, radius * sin(th)])
			scene['eye'] = camera['eye'] - camera['center']
			print 'Using origin: ' + array_str(camera['eye'], suppress_small = True) + ", saving to " + scene['filename']
			raytracer( camera, scene, screen ) # TODO: multithreading for parallel processing?
			theta = theta + 45#90.
	else:
		raytracer( camera, scene, screen )

if __name__ == "__main__":
	sys.exit(main())
	 */
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

	/**
	 * def raytracer( camera, scene, screen ):
	# First, create an Image
	w = screen['width']
	h = screen['height']
 	image = CreateImage( h, w, 3 )

 	uvw = CreateCoordinateFrame( camera )

 	# Then, trace the scene pixel-by-pixel
 	i = 0.5
 	count = 0
 	while i < h:
 	#for i in xrange(0, h):
 		if count % 50 == 0: print 'row: ' + str(count)
 		j = 0.5
 		while j < w:
 		#for j in xrange(0, w):
 			#print str(i) + " " + str(j) + ' begin\n'
 			# Create a ray from the camera through this pixel
 			ray = RayThroughPoint( camera, uvw, w, h, i, j )

 			color = Trace( 0, 1., scene['refractiveindex'], ray, scene )
 			image[i][j] = color
 			#r,g,b = FindColor(hit)
 			#image[i][j][0] = r
 			#image[i][j][1] = g
 			#image[i][j][2] = b
 			j = j + 1.
 		count = count + 1
 		i = i + 1.
 	# Finally, save the Image
 	SaveImage( image, scene['filename'], w,h )
	 */
}

Color Trace( Ray ray, World world, int depth, float weight, float refractiveindex)
{
	Intersection hit = world.Intersect( ray );

	Color color = FindColor( ray, world, hit, depth, weight, refractiveindex  );

	return color;

	/**
	 * def Trace( depth, weight, n1, ray, scene ):
	# Determine what the ray hits in the world
	hit = Intersect( ray, scene )

	# Determine how to color the pixel
	color = FindColor(ray, depth, weight, n1, hit, scene)

	return color
	 */
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

		Direction vL;
		float light_dist;
		lighting.vL(*light, hit, vL, light_dist );

		float NdotL = glm::dot( (glm::vec4)hit.normal, (glm::vec4)vL );

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
	/**
	 *
def FindColor( ray, depth, weight, n1, hit, scene ):
	if not hit['gothit']:
		if depth == 0: colorval = 128.
		else: colorval = 0.1#depth / 1000.
		return array([colorval, colorval, colorval])

	color = Lighting(ray, depth, weight, n1, hit, scene)
	return color
	 */
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
