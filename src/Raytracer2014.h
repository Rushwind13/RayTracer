/*
 * Raytracer2014.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef RAYTRACER2014_H_
#define RAYTRACER2014_H_

#include "glm/glm.hpp"
typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ;
typedef glm::vec3 vec3 ;
typedef glm::vec4 vec4 ;
const double tau = 6.283185307179586;
const double deg2rad = tau / 360.0;

typedef glm::vec3 Color;
typedef glm::vec3 Point;

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
def raytracer( camera, scene, screen ):
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

def Trace( depth, weight, n1, ray, scene ):
	# Determine what the ray hits in the world
	hit = Intersect( ray, scene )

	# Determine how to color the pixel
	color = FindColor(ray, depth, weight, n1, hit, scene)

	return color

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

def CreateCoordinateFrame( camera ):
	a = subtract( camera['eye'], camera['center'] )
	b = camera['up']

	#w = a/|a|
	w = a / la.norm(a)

	#u = b x w / |b x w|
	u = cross( b, w )
	u = u / la.norm(u)

	#v = w x u
	v = cross( w, u )

	return array([u,v,w])

def RayThroughPoint( camera, uvw, w,h, i,j ):
	global camera_dist
	origin = camera['eye'] - camera['center']
	camera_dist = la.norm(origin)
	#origin = origin / camera_dist
	fovy = camera['fovy']
	fovx = camera['fovx']
	alpha = math.tan(fovx) * ((j - (w/2.0))/(w/2.0) )* w/h
	beta = math.tan(fovy) *((i - (h/2.0))/(h/2.0) )# * h/w
	#print array([alpha,beta])

	direction = alpha * uvw[0] - beta * uvw[1] - uvw[2]
	direction = direction / la.norm(direction)

	return array([origin,direction])

def Shadow( ray, scene, from_object, old_hit, clip_dist ):
	hit = Intersect( ray, scene, from_object, clip_dist )
	#if hit['gothit']:# and from_object['id'] == hit['object']['id']:
	#	print ray
	#	print old_hit
	#	print hit
	#	print '\n'
	#else: print '0'
	return hit['gothit']

from copy import deepcopy
def Intersect( ray, scene, from_object=None, clip_dist=1000000 ):
	#print "in intersect"
	# just hardcode sphere intersection for the moment
	hit = {'gothit':False, 'distance':clip_dist}
	#print ("a")
	self_epsilon = .1

	for o in scene['object']:
		self_test = False
		if from_object:
			#print hit
			#print ray
			# No intersecting with yourself
			if from_object['id'] == scene['object'][o]['id']: self_test = True #continue
			# No intersecting with light sources ("emissive" objects)
			# (only when checking for shadow calc)
			if 'emissive' in scene['object'][o]['material']: continue
		try:
			func = getattr( sys.modules[__name__], scene['object'][o]['type'] )
		except AttributeError:
			print 'function not found "%s" (%s)' % (scene['object'][o], scene['object'][o]['type'])
		else:
			new_hit = func(ray, scene['object'][o])
			#print "after func"
			global camera_dist
			if new_hit['gothit'] and new_hit['distance'] < hit['distance']:
				if self_test:
					if abs(new_hit['distance']) < self_epsilon:
						#print "epsilon hit"
						continue
				hit = deepcopy(new_hit)
				if from_object:
					#print "shadow hit"
					break # shadows only need to have one intersection, not just the closest one.

	#if hit['gothit']: print hit
	#else: print 'no hit'
	return hit

def sphere( ray, object ):
	center = object['position']
	r = object['radius']
	p0c = subtract( ray[0], center )

	# sphere intersection devolves into a quadtratic at^2 + bt + c = 0
	a = dot(ray[1], ray[1]) # P1 . P1
	b = 2. * dot( ray[1], p0c )
	c = dot( p0c, p0c ) - ( r * r )

	# b^2 - 4ac
	discriminant = (b*b) - (4. * a * c)

	if discriminant <= 0.: return {'gothit':False}

	disc_sqrt = sqrt(discriminant)

	# TODO: Note there are some efficiencies that can be done here,
	# because we know that a will not be negative (it's a length squared)
	# so only the numerator could make the thing negative...

	# You got some kind of hit
	root1 = (-b + sqrt(discriminant)) / (2. * a)
	root2 = (-b - sqrt(discriminant)) / (2. * a)

	if root1 == root2: return {'gothit':False} #tangent

	# probably a more efficient way to do this too
	distance = min( root1, root2 )
	if distance < 0: # you are inside the object, don't intersect with it.
		return {'gothit':False}
		#distance = max( root1, root2 )

	global camera_dist
	#distance = distance + camera_dist
	intersection = ray[0] + (ray[1] * distance)

	# for lighting, later.
	normal = subtract( intersection, center )
	normal = normal / la.norm(normal)
	normal = normal # should be +normal

	u = sqrt( normal[0]*normal[0] + normal[2]*normal[2] ) * 2. # multiply by 2 because X goes 360 where Y goes 180
	v = arctan2(normal[2],normal[0])
	texture = array([u,v])
	#print intersection
	#print center
	#print normal
	#print '\n'

	return {'gothit':True, 'object': object, 'distance':distance, 'intersection': intersection, 'normal':normal, 'texture':texture}

def box( ray, object ):
	#print "in box"
	p1 = object['p1']
	p2 = object['p2']

	Tnear = -1000000.
	Tfar = 1000000.

	hitp1=[False,False,False]
	for axis in (0,1,2):
		# generalized axis check
		d = ray[1][axis] # direction of ray for this axis
		o = ray[0][axis] # origin of ray for this axis
		# p1[axis] maintains whether the p1 or the p2 plane was hit
		(l, h, hitp1[axis]) = ( p1[axis],p2[axis], True) #(axis != 2) ) # points on the box for this axis
		if l > h : (l,h, hitp1[axis]) = (h,l, not hitp1[axis]) # swap hi and lo. Also, it was p2.

		#print("2")
		#print d
		if d == 0.0: # parallel to axis, so just check origin between planes
			if o < l or o > h:
				#print "o not between planes on axis: " + str(axis)+ " " + str(l) + " "+ str(o) + " "  + str(h)
				return {'gothit':False}
			else:
				D = 1.
		else:
			D = 1. / d
		#print("3")
		T1 = (l - o) * D
		T2 = (h - o) * D
		if T1 > T2: (T1,T2, hitp1[axis])=(T2,T1, hitp1[axis]) # T1 is intersection with near plane
		if T1 > Tnear:
			#print T1
			Tnear = T1 # want largest Tnear
			nearaxis = axis # if you move Tnear, then you know which axis you hit
		if T2 < Tfar:
			#print T2
			Tfar  = T2 # want smallest Tfar
		if Tnear > Tfar:
			#print "Tnear > Tfar on axis: " + str(axis) + " "+ str(Tnear) + " "  + str(Tfar)
			return {'gothit':False} # box missed
		#print("4")
		if Tfar < 0:
			#print "Tfar < 0 on axis: " + str(axis) + " "+ str(Tfar)
			return {'gothit':False} # ray behind box

	# if you get here, you hit the box at Tnear (and exited at Tfar)
	# also, with hitp1 and nearaxis, you can figure out which of the 6 sides you hit (assume axis-aligned for now, transformations later):
	distance = Tnear

	#print "distance: " + str(distance)

	if distance < 0.: # you are inside object, don't intersect with it (until you do bounding boxes later)
		return {'gothit':False}

	#distance = distance + camera_dist
	intersection = ray[0] + (ray[1] * distance)

	# create a normal
	# Except for the Z axis (because we're looking down -Z), if you hit the "lower" value, the normal goes more negative.
	normal = array([0.,0.,0.])
	epsilon = 0.1
	if intersection[nearaxis] - p1[nearaxis] < epsilon:
		normal[nearaxis] = p1[nearaxis]-p2[nearaxis]
	elif intersection[nearaxis] - p2[nearaxis] < epsilon:
		normal[nearaxis] = p2[nearaxis]-p1[nearaxis]
	else:
		d1 = intersection[nearaxis] - p1[nearaxis]
		d2 = intersection[nearaxis] - p2[nearaxis]
		print array([nearaxis, intersection[nearaxis], p1[nearaxis], p2[nearaxis], d1, d2])
	if la.norm(normal) == 0.:
		print intersection
		print nearaxis
		print p1
		print p2
		print '\n'
	normal = normal / la.norm(normal)
	#if hitp1[nearaxis]: normal[nearaxis] = -1.
	#else: normal[nearaxis] = 1.

	# create texture coordinates
	# lerp between min/max on the face (note you know which axis you hit, so just use the other axes)
	u = 0.
	v = 0.
	if nearaxis == 0: # hit x axis, use y,z
		u = lerp(intersection[2], p2[2], p1[2])
		v = lerp(intersection[1], p2[1], p1[1])
	elif nearaxis == 1: # hit y axis, use x,z
		u = lerp(intersection[0], p2[0], p1[0])
		v = lerp(intersection[2], p2[2], p1[2])
	else: # hit z axis, use x,y
		u = lerp(intersection[0], p2[0], p1[0])
		v = lerp(intersection[1], p2[1], p1[1])
	texture = array([u,v])
	#print hitp1
	#print array([nearaxis, Tnear, Tfar])
	#print array([ray[1], intersection])
	#print distance
	#print intersection
	#print normal
	#print '\n'

	return {'gothit':True, 'object': object, 'distance':distance, 'intersection': intersection, 'normal':normal, 'texture':texture}

def lerp( point, min, max ):
	return point / (max - min)

def FindColor( ray, depth, weight, n1, hit, scene ):
	if not hit['gothit']:
		if depth == 0: colorval = 128.
		else: colorval = 0.1#depth / 1000.
		return array([colorval, colorval, colorval])

	color = Lighting(ray, depth, weight, n1, hit, scene)
	return color

def Lighting( ray, depth, weight, n1, hit, scene ):
	# I = A + E + sum(i, Vi * (Li / (c0+c1r+c2r^2)) * ( D * max(N . L, 0) + S * max(N . H, 0)^s)
	# I = final intensity
	# A = ambient
	# E = self-emission
	# D = diffuse (summed over all lights i)
	# S = specular (summed over all lights i)
	# Li = intensity of light i
	# vN = surface normal
	# vL = direction to light
	# vH = half-angle (?)
	# s = shininess
	# Vi = visibility to light i (raytracers only); for shadows
	# c0..c2 = attenuation (set to 1,0,0 for no attenuation or 0,0,1, like a physical point light)
	material = hit['object']['material']

	A = array([0.,0.,0.])
	E = array([0.,0.,0.])
	D = array([0.,0.,0.])
	S = array([0.,0.,0.])
	vN = hit['normal']
	texture = False
	if 'ambient' in material:	A = array(material['ambient'])
	if 'emissive' in material:	E = array(material['emissive'])
	if 'diffuse' in material:	D = array(material['diffuse'])
	if 'reflection' in material:	S = array(material['reflection'])
	if 'texture' in material: texture = material['texture']
	s = 100.

	# using the hit direction as the "eye" direction -- I don't think this is right
	vHit = hit['intersection']
	vHit = vHit
	#print vHit
	pEye = ray[0] #scene['eye']
	vDir = ray[1]
	#vEye = vEye - vHit
	#vEye = vEye / la.norm(vEye)
	#print vEye
	#vEye = vEye

	phong = True
	phong_blinn = False
	gloss = phong or phong_blinn
	ambient = True
	emissive = True
	lambert = True
	shadow = True
	reflection = True
	refraction = False
	recursion = reflection or refraction
	fog = False

	if refraction:
		if n1 == scene['refractiveindex']:
			# transition from scene into an object
			transparency = material['refraction']['t']
			n2 = material['refraction']['n']
			Wt = 1. - transparency
		else:
			# transition from "inside" an object back into the scene
			transparency = 0.
			n2 = scene['refractiveindex']
			vN = -vN
			Wt = 1.
	else:
		Wt = 1.
		transparency = 0.
		n2 = n1

	#if not reflection:
	Wr = 1.
	reflectivity = 0.

	###
	# AMBIENT & EMISSIVE
	###
	I = array([0.,0.,0.])
	if ambient: I = I + A
	if emissive and (E != array([0.,0.,0.])).any(): I = I + E
	else:
		# Now that you have the background lights on, loop through the
		# world lights to determine 3D shading
		for light in scene['lights']:
			# is the "direction to the light" relative to the hit point, or just its position in space?
			vL = array(scene['lights'][light]['position']) - vHit
			#vL = scene['lights'][light]['position']
			light_dist = la.norm(vL)
			vL = vL / light_dist

			# first, check to see whether this light can possibly affect this point
			NdotL = dot(vN,vL)
			if NdotL < 0.: continue # cos < 0 means light coming from below surface.

			if shadow:
				# cast shadow ray
				vOrigin = deepcopy(vHit)
				vDir = deepcopy(vL)
				rShadow = array([vOrigin, vDir, vN])
				shadowed = Shadow( rShadow, scene, hit['object'], hit, light_dist )
				if shadowed:
					#print "shadowed"
					continue

			# Not shadowed, so calculate the diffuse and specular components
			if 'color' in scene['lights'][light]: Li= array(scene['lights'][light]['color'])
			else: Li = array([0.,0.,0.])

			if fog:
				#print light_dist
				fog_falloff = 1. / (light_dist)# * light_dist)
				#print fog
				Li = Li * fog_falloff
				#print Li
				#print

			###
			# DIFFUSE, matte
			###
			if lambert:
				if texture: tex = checkerboard( hit['texture'] )
				else: tex = 1.
				diffuse = D * tex * NdotL # "cos falloff", scale D between 0..D
			else:
				diffuse = array([0.,0.,0.])

			###
			# SPECULAR, glossy
			###

			# gloss based on reflected . eye
			if gloss:
				#vViewer = pEye - vHit
				vViewer = vDir
				if phong:
					vR = ReflectVector( vL, vN )
					cosTheta = dot( vR, vViewer ) # Should this be vHit instead?
					#print cosTheta
					if cosTheta < 0.: cosTheta = 0.
				# gloss based on half-angle . normal
				elif phong_blinn:
					vH = vL + vViewer
					len = la.norm(vH)
					if len != 0. :
						vH = vH / len
						cosTheta = dot( vN, vH )
						#print cosTheta
						if cosTheta < 0. : cosTheta = 0.
					else:
						# Not sure what would make len = 0 but if it does, no gloss for you
						cosTheta = 0.

				specular = S * pow( cosTheta, s ) # similar cos falloff, but use s for specular spot size
				if reflection:
					reflectivity = S[0] # for now, tie "reflection" to the specular color (the idea being that more specular stuff reflects more)
					Wr = 1. - reflectivity
					Li = Li * Wr # ...and reflective surfaces don't have their own color, just the stuff that's reflected
			# or turn specular off
			else:
				specular = array([0.,0.,0.])

			# Add the contribution from this light
			#print diffuse
			#print specular
			#print Li
			#print light
			#print depth
			#print hit
			#print
			I = I + (Li * (diffuse + specular))
		I = I * weight * Wr * Wt # modify local color for reflection and transparency
		# TODO: Now that you've got the local color, do any reflection and refraction
		if( recursion and depth + 1 < scene['maxdepth'] ):
			minweight = scene['minweight']
			if( reflection and reflectivity*weight > minweight ):
				#distance = la.norm(vHit)
				#vHit = vHit / distance
				vR = ReflectVector( vDir, vN )
				#print vDir
				#print vN
				#print
				#vR = vR * distance
				rcolor = Trace( depth+1, weight * reflectivity, n2, array([vHit,vR]), scene)
				I = I + reflectivity * rcolor
				#print "rcolor: " + array_str(rcolor, suppress_small=True)
			if( refraction and transparency*weight > minweight ):
				#distance = la.norm(vHit)
				#vHit = vHit / distance
				vT = RefractVector(vDir, vN, n1, n2)
				if (vT != array([0.,0.,0.])).any():
					tcolor = Trace( depth+1, weight * transparency, n2, array([vHit,vT]), scene)
					I = I + transparency * tcolor
					#print tcolor
				else:
					#print "got here"
					I = array([1.,0.,1.])
			#print I
			#print '\n'

	if depth == 0:
		#print '\n'
		# Now that we've looped through all the lights,
		# we need to clamp the color to 0..255 in each channel
		#print array_str(I, precision=1, suppress_small=True)
		clip( I, 0., 1., out=I)
		I = I * 255
		#print I

	return I

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

# Tested and working properly
def ReflectVector( vIncident, vNormal ):
	# vR = vI - [2 * (N . I)]N
	NScalar = 2. * dot( vIncident, vNormal )
	vNscaled = multiply(vNormal, NScalar)
	vReflected = vNscaled - vIncident
	vReflected = vReflected / la.norm(vReflected)

	return vReflected

# Tested and working properly
def RefractVector( vIncident, vNormal, n1, n2 ):
	# vT = (n1/n2)vI - [cosThetat * (n1/n2)(N . I)]N
	# cosThetat = angle between transmitted ray and -N

	eta_ratio = n1 / n2
	IdotN = dot( vIncident, vNormal )

	# man, what a mess. root of 1 - [ ratio^2 * ( 1 - (dotprod^2) ) ]
	ct1 = eta_ratio * eta_ratio
	ct2 = 1 - (IdotN * IdotN)
	if ct1*ct2 > 1.: return array([0.,0.,0.])
	cosThetat = sqrt(1 - (ct1*ct2))
	Nscalar = cosThetat + (eta_ratio * IdotN)

	vIscaled = vIncident * eta_ratio
	vNscaled = vNormal * Nscalar

	vRefracted = vIscaled - vNscaled
	vRefracted = vRefracted / la.norm(vRefracted)

	return vRefracted

    # Now run the raytracer
    # eye = array([0., 0., 5.])
#     center = array([0.,0.,0.])
#     up = array([0.,1.,0.])
#     width = 200.0
#     height = 100.0
#     fovy = 90.0
#     fovx = fovy * (width / height)
#     camera = {'eye': eye, 'center':center, 'up':up, 'fovy':fovy, 'fovx':fovx}
#
#     loc = array([0.,4.,-8.])
#     color = array([0xff,0x00,0x00])
#     sphere = { 'location': loc, 'color': color, 'radius': 10.0 }
#     scene = { 'object': sphere, 'filename':'rgb.png' }
#     screen = {'width':int(width), 'height':int(height)}


def create_camera( metadata ):
	camera = {}
	camera['eye'] = array(metadata['camera']['pOrigin'])
	camera['center'] = array(metadata['camera']['pLookAt'])
	camera['up'] = array(metadata['camera']['nUp'])
	camera['fovy'] = metadata['camera']['fov']
	w = metadata['world']['size']['w']
	h = metadata['world']['size']['h']
	fovy = deg2rad(metadata['camera']['fov'])
	fovx = fovy# * ( w/h ))
	camera['fovy'] = fovy
	camera['fovx'] = fovx
	return camera

def create_screen( metadata ):
	screen = {}
	screen['width'] = metadata['world']['size']['w']
	screen['height'] = metadata['world']['size']['h']
	return screen

def create_scene( metadata ):
	scene = {}
	scene['filename'] = metadata['world']['filename']
	scene['object'] = metadata['objects']
	scene['lights'] = metadata['lights']
	scene['eye'] = array(metadata['camera']['pOrigin']) - array(metadata['camera']['pLookAt'])
	scene['refractiveindex'] = metadata['world']['background']['refraction']['n']
	scene['maxdepth'] = metadata['world']['maxdepth']
	scene['minweight'] = metadata['world']['minweight']

	return scene

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
 *
 */
