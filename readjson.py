#!/usr/bin/python
#
# readfile.py - simple commandfile reader for raytrace/graphics engine
#
# Created 3/14/2013 by Jimbo S. Harris
# to read file format for CS184 edX course
#

import json

def parse( filename ):
	with open(filename) as f:
		d = f.read()
	metadata = json.loads(d)
	return metadata