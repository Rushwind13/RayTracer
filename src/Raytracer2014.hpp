/*
 * Raytracer2014.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef RAYTRACER2014_H_
#define RAYTRACER2014_H_

void SaveImage( const Color *image, const World world, const Camera camera );
Color Trace( Ray ray, World world, int depth, float weight, float refractiveindex);
Color FindColor( Ray ray, World world, Intersection hit, int depth, float weight, float refractiveindex );
void raytracer( Camera camera, World world );

#endif /* RAYTRACER2014_H_ */
/**
 * #!/usr/bin/python
#
# Raytrace.py - a simple ray tracing engine in Python
#
# Created 3/14/2013 (Happy Pi Day) by Jimbo S. Harris
# based on algorithms learned in CS184 from edX
#
#import Image
from numpy import *
from numpy import linalg as la

import readjson

#####
#
#  RAYTRACE ROUTINES
#
#####
camera_dist = 0.




def CreateImage( h, w, planes ):
	return zeros((h,w,planes), 'uint8')

import png
def SaveImage( image, filename, w,h ):
	#print image

	# add scipy to easily scale the data to uint8...
	# scipy.misc.bytescale( image )
	file = open( filename, 'wb' )

	writer = png.Writer( w, h, bitdepth=8)
	img = image.reshape(image.shape[0], image.shape[1]*image.shape[2])

	writer.write(file, img)

	file.close()




from copy import deepcopy



def checkerboard( texture ):
	u = texture[0]
	v = texture[1]

	checkers = 12. * pi
	u = u - 0.49
	v = v - 0.49
	#v = v + 0.76 #works for cos(v)
	x = sin(checkers*u)
	y = sin(checkers*v)

	debug = False
	if debug:
		if cmp(x,y) > 0: return 1
		else: return 0
	x = cmp(x,0)
	y = cmp(y,0)
	out = 0
	if   y == 0:
		if x < 0: out = 1
	elif x == 0:
		if y < 0: out = 1
	else:
		if x != y: out = 1

	#print array([x,y]),
	return out

/**
 *


    # Now run the raytracer
    #
#
#     loc = array([0.,4.,-8.])
#     color = array([0xff,0x00,0x00])
#     sphere = { 'location': loc, 'color': color, 'radius': 10.0 }
#     scene = { 'object': sphere, 'filename':'rgb.png' }
#     screen = {'width':int(width), 'height':int(height)}
 *
 */

