#!/usr/bin/python
#
# readfile.py - simple commandfile reader for raytrace/graphics engine
#
# Created 3/14/2013 by Jimbo S. Harris
# to read file format for CS184 edX course
#

import sys
from numpy import *

lightnumber = 0
maxverts = 0
vertnumber = 0
objectnumber = 0
trinumber = 0

def size(metadata, args):
	metadata['width'] = int(args[0])
	metadata['height'] = int(args[1])
	
def maxdepth(metadata, args):
	metadata['maxdepth'] = int(args[0])
		
def minweight(metadata, args):
	metadata['minweight'] = float(args[0])
		
def refractiveindex(metadata, args):
	metadata['refractiveindex'] = float(args[0])
		
def transparency(metadata, args):
	metadata['transparency'] ={}
	metadata['transparency']['weight'] = float(args[0])
	metadata['transparency']['refractiveindex'] = float(args[1])
	
def filename(metadata, args):
	metadata['filename'] = str(args[0])

def camera(metadata, args):
	metadata['camera'] ={}
	metadata['camera']['eye'] = array(map(float, args[0:3]))
	metadata['camera']['center'] = array(map(float, args[3:6]))
	metadata['camera']['up'] = array(map(float, args[6:9]))
	metadata['camera']['fovy'] = float(args[9])

# The Material properties will affect the next object to be created,
# and will continue to apply to further objects until they are 
# overwritten. A bit of a hack, but that's the way the original file was written.
# TODO: redo the input file as JSON
def ambient(metadata, args):
	if not 'material' in metadata: metadata['material'] ={}
	color = array(map(float, args[0:3]))
	if (color!=array([0.,0.,0.])).any():
		metadata['material']['ambient'] = color
	else:
		if 'ambient' in metadata['material']: del metadata['material']['ambient']
	
def diffuse(metadata, args):
	if not 'material' in metadata: metadata['material'] ={}
	color = array(map(float, args[0:3]))
	if (color!=array([0.,0.,0.])).any():
		metadata['material']['diffuse'] = color
	else:
		if 'diffuse' in metadata['material']: del metadata['material']['diffuse']
	
def specular(metadata, args):
	if not 'material' in metadata: metadata['material'] ={}
	color = array(map(float, args[0:3]))
	if (color!=array([0.,0.,0.])).any():
		metadata['material']['specular'] = color
	else:
		if 'specular' in metadata['material']: del metadata['material']['specular']
	
def emissive(metadata, args):
	if not 'material' in metadata: metadata['material'] ={}
	color = array(map(float, args[0:3]))
	if (color!=array([0.,0.,0.])).any():
		metadata['material']['emissive'] = color
	else:
		if 'emissive' in metadata['material']: del metadata['material']['emissive']
		


def directional(metadata, args):
	global lightnumber
	
	if not 'light' in metadata: metadata['light'] ={}
	if not lightnumber in metadata['light']: metadata['light'][lightnumber] ={}
	
	metadata['light'][lightnumber]['direction'] = array(map(float, args[0:3]))
	metadata['light'][lightnumber]['color'] = array(map(float, args[3:6]))
	metadata['light'][lightnumber]['type'] = 'directional'
	lightnumber = lightnumber + 1

def point(metadata, args):
	global lightnumber
	
	if not 'light' in metadata: metadata['light'] ={}
	if not lightnumber in metadata['light']: metadata['light'][lightnumber] ={}
	
	metadata['light'][lightnumber]['direction'] = array(map(float, args[0:3]))
	metadata['light'][lightnumber]['color'] = array(map(float, args[3:6]))
	metadata['light'][lightnumber]['type'] = 'point'
	lightnumber = lightnumber + 1

def light( metadata, args ):
	global lightnumber
	
	if not 'light' in metadata: metadata['light'] ={}
	if not lightnumber in metadata['light']: metadata['light'][lightnumber] ={}
	
	metadata['light'][lightnumber]['direction'] = array(map(float, args[0:3]))
	metadata['light'][lightnumber]['color'] = array(map(float, args[3:6]))
	metadata['light'][lightnumber]['type'] = 'light'
	lightnumber = lightnumber + 1

# Object properties
# Again, these will sort of hang around outside the context of this 
# line parsing.
def maxverts( metadata, args ):
	global maxverts
	maxverts = int(args)
	
def vertex( metadata, index, args ):
	global vertnumber
	global objectnumber
	
	if not 'object' in metadata: metadata['object'] ={}
	if not objectnumber in metadata['object']: metadata['object'][objectnumber] ={}
	if not 'vertex' in metadata['object'][objectnumber]: metadata['object'][objectnumber]['vertex'] ={}
	metadata['object'][objectnumber]['vertex'][vertnumber]= array(map(float, args[0:3]))
	vertnumber = vertnumber + 1

def tri( metadata, index, args ):
	global trinumber
	global objectnumber
	
	if not 'object' in metadata: metadata['object'] ={}
	if not objectnumber in metadata['object']: metadata['object'][objectnumber] ={}
	if not 'tri' in metadata['object'][objectnumber]: metadata['object'][objectnumber]['tri'] ={}
	
	metadata['object'][objectnumber]['tri'][trinumber] = array(map(float, args[0:3]))
	trinumber = trinumber + 1
	
from copy import deepcopy
def sphere( metadata, args ):
	objectnumber = object( metadata, args )
	metadata['object'][objectnumber]['type'] = 'sphere'
	metadata['object'][objectnumber]['location'] = array(map(float, args[0:3]))
	metadata['object'][objectnumber]['radius'] = float(args[3])
	
def box( metadata, args ):
	objectnumber = object( metadata, args )
	metadata['object'][objectnumber]['type'] = 'box'
	metadata['object'][objectnumber]['p1'] = array(map(float, args[0:3]))
	metadata['object'][objectnumber]['p2'] = array(map(float, args[3:6]))

def object( metadata, args ):
	global objectnumber
	
	if not 'object' in metadata: metadata['object'] ={}
	if not objectnumber in metadata['object']: metadata['object'][objectnumber] ={}
	metadata['object'][objectnumber]['id'] = int(objectnumber)	
	metadata['object'][objectnumber]['material'] = deepcopy(metadata['material'])
	metadata['object'][objectnumber]['transparency'] = deepcopy(metadata['transparency'])
	objectnumber = objectnumber + 1
	
	return objectnumber - 1

def parse( filename, metadata ):
	f = open( filename, 'r')
	for line in f:
		#print line
		line = line.strip()
		if not line: continue
		if line[0] == '#': continue
		if line[0] == '\n': continue
		words = line.split(' ')
		try:
   			func = getattr(sys.modules[__name__], words[0])
		except AttributeError:
			print 'function not found "%s" (%s)' % (words[0], words[1:])
		else:
			func(metadata, words[1:])
	print 'Found %d objects, %d lights' % (objectnumber, lightnumber)
# sample file
## Test metadata 1 
## A simple quad viewed from different camera positions
#
#size 640 480 
#
## Now specify the camera.  This is what you should implement.
## This file has 4 camera positions.  Render your metadata for all 4.
#
##camera 0 0 4 0 0 0 0 1 0 30
##camera 0 -3 3 0 0 0 0 1 0 30
##camera -4 0 1 0 0 1 0 0 1 45
#camera -4 -4 4 1 0 0 0 1 0 30
#
## lighting/material definitions
## for initial testing, you should get the geometry right
## you may want to use dummy (constant) shading, ignoring these lines
#
#ambient .1 .1 .1
#directional 0 0 1 .5 .5 .5 
#point 4 0 4 .5 .5 .5
#diffuse 1 0 0 
#specular 0 0 0
#
## Now specify the geometry 
## Again, for initial testing, maybe you want to implement a dummy 
## ray-plane intersection routine, instead of reading this fully.
## Though it's probably simpler you just implement this, for later 
## parts of the assignment.
#
#maxverts 4 
#
#vertex -1 -1 0 
#vertex +1 -1 0
#vertex +1 +1 0
#vertex -1 +1 0
#
#tri 0 1 2
#tri 0 2 3
#
#sphere 1 0 0 0.15