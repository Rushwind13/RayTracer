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

class Material
{
public:
	Material()
	{
		std::cout << "Material()...";
		color = COLOR_RED;
		ambient = 0.1;
		diffuse = 0.9;
		specular = 0.9;
		shininess = 200.0;
		isReflective = true;
	}
	Color color;
	float ambient;
	float diffuse;
	float specular;
	float shininess;
	bool isReflective;
};

class Object
{
public:
	Object()
	{
		std::cout << "Object()...";
		SetTransform(glm::mat4(1.0));
	};
	Object( const glm::mat4 o2w )
	{
		std::cout << "Object(o2w)...";
		SetTransform(o2w);
		//material (diffuse, specular, transparency, translucency)
	}
	virtual ~Object() {}
	virtual bool local_intersect( const Ray &object, Intersection &i ) = 0;
	virtual Direction local_normal_at( const Position object_pos ) const = 0;

	bool Intersect( const Ray &world, Intersection &i )
	{
		const Ray object = TransformRay(world, worldToObject);
		return local_intersect(object,i);
	}

	void SetTransform( const glm::mat4 o2w )
	{
		objectToWorld = o2w;
		worldToObject = glm::inverse(objectToWorld);
		normalToWorld = glm::transpose(worldToObject);
	}

	Direction NormalAt( const Direction world_pos ) const
	{
		Direction world_normal;
		Direction object_normal;

		Position object_pos;

		object_pos = worldToObject * world_pos;
		object_normal = local_normal_at(object_pos);

		world_normal = normalToWorld * object_normal;
		world_normal.w = 0.0;

		return glm::normalize(world_normal);
	}

	glm::mat4 objectToWorld, worldToObject, normalToWorld;
	Material material;
	short oid = -1;
	std::string name;
};

class Light: public Object
{
public:
	Light(Position pos) : position(pos) {}
	//Light() {}
	~Light() {}

	bool local_intersect( const Ray &object, Intersection &i ) const
	{
		// lights don't intersect as geometry
		return false;
	}
	Direction local_normal_at( const Position object_pos ) const
	{
		// lights don't have normals
		return Direction(0);
	}

	Position position;
};

class Sphere : public Object
{
public:
	Sphere()
	{
		std::cout << "Sphere()" << std::endl;
		center = Position(0);
		radius = 1.0;
		radius2 = 1.0;
	}

	Sphere( Position c, float r = 1 )
	{
		std::cout << "Sphere(c,r)" << std::endl;
		Position scalar(r);
		// scalar.y /= 2.0;
		glm::mat4 scale = ScaleMatrix(scalar);
		glm::mat4 translate = TranslateMatrix(c);
		SetTransform(translate * scale);
		center = Position(0);
		radius = 1.0;
		radius2 = 1.0;
	}

	Sphere( const glm::mat4 o2w )
	{
		std::cout << "Sphere(o2w)" << std::endl;
		SetTransform(o2w);
		center = Position(0);
		radius = 1.0;
		radius2 = 1.0;
	}
	#define MATRIX
	#ifdef MATRIX
	bool local_intersect( const Ray &object, Intersection &i )
	{
		Direction eye = object.origin - center;
		Direction dir = object.direction;
		float a = glm::dot((glm::vec4)dir, (glm::vec4)dir);
		float b = glm::dot((glm::vec4)eye, (glm::vec4)dir);
		float c = glm::dot((glm::vec4)eye, (glm::vec4)eye) - 1.0;

		float b2 = b * b;
		float ac = a * c;

		if( b2 < ac )
		{
			i.distance = 1e9;
			i.gothit = false;
			return false;
		}

		float discriminant = std::sqrt(b2 - ac);
		float root1 = (-b + discriminant) / a;
		float root2 = (-b - discriminant) / a;

		float distance = std::min(root1, root2);
		if( distance < 0.0 )
		{
			// TODO: deal with internal reflection later
			i.distance = 1e9;
			i.gothit = false;
			return false;
		}

		i.distance = distance;
		i.position = objectToWorld * object.apply(i.distance);
		i.normal = NormalAt(i.position);
		// to remove off-by-ε errors, bump the hit position along the normal by a small amount...
		// i.position = i.position + (i.normal * epsilon);
		i.oid = oid;
		i.gothit = true;

		return true;
	}

	Direction local_normal_at( const Position object_pos ) const
	{
		return object_pos - center;
	}

	#else
	bool local_intersect( const Ray &object, Intersection &i )
	{
		// std::cout << "local_intersect" << std::endl;
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

		// printvec( "o", r.origin );
		// printvec( "d", r.direction );
		Direction oc = r.origin - center;
		float DdotOC = glm::dot( (glm::vec4)r.direction, (glm::vec4)oc );
		float len2 = glm::dot((glm::vec4)oc, (glm::vec4)oc);

		// std::cout << DdotOC << std::endl;

		float b = DdotOC;
		float c = len2 - radius2;

		float b2 = b * b;

		// No intersect if miss or tangent
		if( b2 < c )
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

		Position dt = r.direction * i.distance;

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

		// std::cout << "done with local_intersect" << std::endl;
		return true;
	}
	#endif // MATRIX
protected:
	Position center;
	float radius, radius2;
};

class Plane : public Object
{
public:
	Plane() {};
	bool local_intersect( const Ray &object, Intersection &i )
	{
		if( glm::abs( object.direction.y - 0.0 ) < epsilon )
		{
			i.distance = 1e9;
			i.gothit = false;
			return false;
		}

    i.distance = -object.origin.y / object.direction.y;
    if( i.distance < 0.0 )
    {
        i.distance = 1e9;
        i.gothit = false;
        return false;
    }

    i.gothit = true;
		return true;
	}
	Direction local_normal_at( const Position object_pos ) const
	{
		return Direction(0,1,0);
	}
};

class Box : public Object
{
public:
	Box( /*const mat4 o2w,*/ const Position c1, const Position c2 ): /*JObject(o2w),*/ corner1(c1), corner2(c2) {}
	bool local_intersect( const Ray &object, Intersection &i )
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
		{}
		~ParametricEquation() {}

		bool local_intersect( const Ray &object, Intersection &i )
		{
			// TODO: write local_intersect function
			// To intersect a ray with a function, need to solve the following:
			// ray = o + dt , where o (vector) is the ray's origin, d (vector) is the direction, and t is the length(scalar)
			// function = (e.g.) cos x
			// so we need o + dt = cos x, solve for t
			// dt = cos x - o
			// t = ( cos x - o ) / d
			// NOTE: After further research, I have found out that thsi can't be directly computed, and Newton's method
			//       should be used. There was a nice paper presented at SIGGRAPH 1985 by Daniel Toth on the algorithm,
			//       and it devolves to iteratively finding bounding boxes (axis aligned and defined as the inverse Jacobian)
			//       that are closer and closer to the accuracy desired. For now (9/9/2019), I am giving up on parametric equations,
			//       because both axis-aligned box intersection needs to be done, and then the algorithm needs to be implemented.
			return false;
		}

		Position position;
		eFunction x, y, z;
	};

	class Cylinder: public Object
	{
	public:
		Cylinder(Position pos) : position(pos) {}
		//Cylinder() {}
		~Cylinder() {}

		bool local_intersect( const Ray &object, Intersection &i )
		{
			// TODO: Implement cylinder intersection
			return false;
		}

		Position position;
	};

	class Cone: public Object
	{
	public:
		Cone(Position pos) : position(pos) {}
		//Cone() {}
		~Cone() {}

		bool local_intersect( const Ray &object, Intersection &i )
		{
			// TODO: Implement Cone intersection
			return false;
		}

		Position position;
	};


	class Torus: public Object
	{
	public:
		Torus(Position pos) : position(pos) {}
		//Torus() {}
		~Torus() {}

		bool local_intersect( const Ray &object, Intersection &i )
		{
			// TODO: Implement Torus intersection
			return false;
		}

		Position position;
	};


	// TODO: Deal with rotation / scaling / translation of primitives (instead of only axis-aligned and centered at origin)
	#endif /* OBJECT_H_ */
