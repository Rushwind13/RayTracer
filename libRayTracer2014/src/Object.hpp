/*
 * Object.h
 *
 *  Created on: Mar 25, 2014
 *      Author: jimbo
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <iostream>
#include "Math.hpp"
#include "Intersection.hpp"

#define COLOR_RED Color(1.0,0.1,0.1);
class Object
{
public:
	Object() {};
	Object( const glm::mat4 o2w ) : objectToWorld(o2w)
	{
		worldToObject = glm::inverse(objectToWorld);
		//color
		//material (diffuse, specular, transparency, translucency)
	}
	virtual ~Object() {}
	virtual bool Intersect( const Ray &r, Intersection &i ) const = 0;
	glm::mat4 objectToWorld, worldToObject;
	Color color;
	short oid = -1;
	std::string name;
	//Position worldpos;
};

/**
 * def Intersect( ray, scene, from_object=None, clip_dist=1000000 ):
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
 */

class Light: public Object
{
public:
	Light(Position pos) : position(pos) {}
	//Light() {}
	~Light() {}

	bool Intersect( const Ray &r, Intersection &i ) const
	{
		// lights don't intersect as geometry
		return false;
	}

	Position position;
};

class Sphere : public Object
{
public:
	Sphere( /*const mat4 o2w,*/ glm::vec3 c, float r = 1 ): /*JObject(o2w),*/ center(c), radius(r), radius2(r*r)
	{
			color = COLOR_RED;
	}
	bool Intersect( const Ray &r, Intersection &i ) const
	{
		//std::cout << "Intersect" << std::endl;
		// basic ray equation = o + dt (origin, direction, length)
		// basic sphere equation = (p-c)^2-r^2 = 0 (point on sphere, center, radius)
		// sub in ray for p:
		// (o + dt - c )^2 - r^2 = 0
		// solve for t:
		// ( dt + (o-c))^2 - r^2 = 0
		// d^2t^2 + 2((o-c)dt) + (o-c)^2 - r^2 = 0
		// but this is of the form At^2 + Bt + C = 0
		// where
		// a = dot(d, d) (xd^2 + yd^2 + zd^2)
		// b = 2( dot( d, (o-c) )
		// c = dot( o-c, o-c ) - r^2
		//
		// Note that a = 1 since d is unit length
		// Note that b^2 = 4 * (the stuff)^2
		//
		// sub into quadtratic eqn:
		// -b +/- sqrt( b^2 - 4 * a * c ) / 2 * a
		// => -b +/- sqrt(4) * sqrt( (the stuff) ^2 - c ) / 2 /* pull out factor of 4 and a=1 */
		//
		// Note b = 2 * (the stuff)
		// => -(the stuff) +/- sqrt( (the stuff) ^2 - c)  /* remove factor of 2 */
		//
		// So, if dot( d, o-c )^2 < c then the sqrt goes complex and there's no intersection
		// If they are equal, then the ray is tangent to the sphere
		// If you have a negative root, then (at least part of) the sphere is behind the ray's origin
		// Other than that, the intersection point (h) is the smaller positive root.
		// The normal at that point is normalize(h-c) or, in a nifty trick, since you have
		// h = o + dt and you have o-c calculated already, (h-c) = o + dt - c = (o-c) + dt
		// which seems counterintuitive (since vec subtraction is not commutative nor associative), but seems to work.

		//printvec( "o", r.origin );
		//printvec( "d", r.direction );
		glm::vec3 oc = r.origin - center;
		float DdotOC = glm::dot( r.direction, oc );
		float len2 = glm::dot(oc, oc);

		//std::cout << DdotOC << std::endl;

		float b = DdotOC;
		float c = len2 - radius2;

		float b2 = b * b;

		// No intersect if miss or tangent
		if( b2 <= c )
		{
			i.distance = 1e9;
			i.gothit = false;
			return false;
		}

		// two intersection points; choose smallest positive one
		float discriminant = std::sqrt( b2 - c );
		float root1 = -b + discriminant;
		float root2 = -b - discriminant;

		i.distance = std::min(root1, root2);
		if( i.distance <= 0.0f )
		{
			// inside the sphere; for now don't intersect
			// but later could use reversed normals.
			i.distance = 1e9;
			i.gothit = false;
			return false;
		}

		glm::vec3 dt = r.direction * i.distance;

		//i.object = reinterpret_cast<const Object *>(this);
		i.oid = oid;
		i.position = r.origin + dt;
		// Note: o + dt - c = (o - c) + dt
		//i.normal = glm::normalize(oc + dt);
		i.normal = glm::normalize(i.position - center);
		i.gothit = true;

		// TODO: texture coordinates
		/*
		 * 	u = sqrt( normal[0]*normal[0] + normal[2]*normal[2] ) * 2. # multiply by 2 because X goes 360 where Y goes 180
			v = arctan2(normal[2],normal[0])
			texture = array([u,v])
		 */

		//std::cout << "done with Intersect" << std::endl;
		return true;
	}
protected:
	glm::vec3 center;
	float radius, radius2;
};

class Box : public Object
{
public:
	Box( /*const mat4 o2w,*/ const Position c1, const Position c2 ): /*JObject(o2w),*/ corner1(c1), corner2(c2) {}
		bool Intersect( const Ray &r, Intersection &i ) const
		{
			return false;
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
protected:
		Position corner1, corner2;
};

// TODO: Comments about parametric equations
enum eFunction
{
	FUNCTION_INVALID=-1,
	FUNCTION_IDENTITY=0,
	FUNCTION_SIN=1,
	FUNCTION_COS=2,
	FUNCTION_X2=3,
	FUNCTION_LN=4,
	FUNCTION_MAX
};
class ParametricEquation: public Object
{
public:
        ParametricEquation(
		Position pos,
		eFunction xFunction = FUNCTION_IDENTITY,
		eFunction yFunction = FUNCTION_IDENTITY,
		eFunction zFunction = FUNCTION_IDENTITY )
	: position(pos), x(xFunction), y(yFunction), z(zFunction)
	{
		color = COLOR_RED;
	}
        ~ParametricEquation() {}

        bool Intersect( const Ray &r, Intersection &i ) const
        {
// TODO: write Intersect function
                return false;
        }

        Position position;
	eFunction x, y, z;
};
#endif /* OBJECT_H_ */
