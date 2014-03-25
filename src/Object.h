/*
 * Object.h
 *
 *  Created on: Mar 25, 2014
 *      Author: jimbo
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include "Math.h"

class Object
{
public:
	Object( const mat4 o2w ) : objectToWorld(o2w)
	{
		worldToObject = objectToWorld.inverse();
		//color
		//material (diffuse, specular, transparency, translucency)
	}
	virtual ~Object() {}
	virtual bool Intersect( const Ray &r, double &t ) const = 0;
	mat4 objectToWorld, worldToObject;
	Color color;
};

class Sphere : public Object
{
public:
	Sphere( const mat4 o2w, double r = 1 ): Object(o2w), radius(r), radius2(r*r) {}
	bool Intersect( const Ray &r, double &t ) const
	{
	/**
	 * def sphere( ray, object ):
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
	 *
	 */

	}
	double radius, radius2;
};

class Box : public Object
{
public:
	Box( const mat4 o2w, const Position c1, const Position c2 ): Object(o2w), corner1(c1), corner2(c2) {}
		bool Intersect( const Ray &r, double &t ) const
		{
/**
 * def box( ray, object ):
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
 *
 */
		}
		Position corner1, corner2;
};
#endif /* OBJECT_H_ */
