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
		//TODO: worldToObject = glm::inverse(objectToWorld);
		//color
		//material (diffuse, specular, transparency, translucency)
	}
	virtual ~Object() {}
	virtual bool Intersect( const Ray &r, Intersection &i ) const = 0;
	glm::mat4 objectToWorld, worldToObject;
    glm::mat3 normalToObject, normalToWorld;
	Color color;
	short oid = -1;
	std::string name;
	//Position worldpos;
};

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
            // r = radius = 1.0;
            // radius2 = r * r;
			color = COLOR_RED;
#define MATRIX
#ifdef MATRIX
            objectToWorld = glm::mat4(r);
            objectToWorld[3] = glm::vec4(c, 1.0);
            worldToObject = glm::inverse(objectToWorld);
			glm::mat4 wtoT = glm::transpose(worldToObject);

            normalToWorld = glm::mat3(r);
            // TODO: simple inverse works under scaling only, but not when rotation is added
            // Nobj = S^-1 * R * Nworld (invert scale but not rotation)
            normalToObject = glm::inverse(normalToWorld);
            center = glm::vec3(0.0, 0.0, 0.0);
#define TRACE
#ifdef TRACE
            glm::vec4 world_center = glm::vec4(c, 1.0);
            glm::vec4 object_center = worldToObject * world_center;
            glm::vec4 world_center_calculated = objectToWorld * object_center;

            glm::vec3 world_normal = glm::vec3(0.0,0.0,1.0);
            glm::vec3 object_normal = normalToObject * world_normal;

            printvec( "Sphere() c", c);
            std::cout << std::endl;
            printvec( "oc", object_center);
            printvec( "expected", glm::vec4(center, 1.0));
            std::cout << std::endl;
            printvec( "wc", world_center_calculated);
            printvec( "expected", world_center);
            std::cout << std::endl;
            printmat( "wto", worldToObject);
            std::cout << std::endl;
            printmat( "otw", objectToWorld);
            std::cout << std::endl;
            printmat( "wTo", wtoT);
            std::cout << std::endl;
            printmat( "nto", normalToObject);
            std::cout << std::endl;
            printmat( "ntw", normalToWorld);
            std::cout << std::endl;
            printvec( "on", object_normal);
            glm::vec3 object_normal_calculated = world_normal;
            object_normal_calculated *= 1.0/r;
            printvec( "expected", object_normal_calculated);
            std::cout << std::endl;
            std::cout << glm::dot( object_normal, object_normal ) << std::endl;

            glm::vec4 world_origin = glm::vec4(0.0, 0.0, 0.0, 1.0);
            glm::vec3 world_direction = glm::vec3(0.0, 0.0, -1.0);

            glm::vec4 object_origin = worldToObject * world_origin;
            glm::vec3 object_direction = normalToObject * world_direction;
            printvec("io", object_origin);
            std::cout << std::endl;
            printvec("id", object_direction);

            float DdotD = glm::dot( object_direction, object_direction );
            float DdotO = glm::dot( glm::vec4(object_direction, 0.0), object_origin );
            float OdotO = glm::dot( object_origin, object_origin );
            float a1 = DdotD;
            float b1 = DdotO;
            float c1 = (OdotO - 2.0);

            float b2 = b1 * b1;
            float discriminant = std::sqrt( b2 - (a1 * c1));
            float root1 = (-b1 + discriminant) / (a1);
            float root2 = (-b1 - discriminant) / (a1);

            float distance = std::min(root1, root2);
            std::cout << std::endl;
            std::cout << "a " << a1 << " b " << b1 << " c " << c1 << std::endl;
            std::cout << "b2 " << b2 << " disc " << discriminant << std::endl;
            std::cout << "r1 " << root1 << " r2 " << root2 << " d " << distance << std::endl;

            glm::vec3 dt = object_direction * distance;
            glm::vec3 object_position = object_origin + glm::vec4(dt, 0.0);
            glm::vec4 out_position = glm::vec4( objectToWorld * glm::vec4(object_position, 1.0));
            glm::vec3 out_normal = object_position;

            printvec( "outpos", out_position );
            std::cout << std::endl;
            printvec( "outnormal", out_normal );
            std::cout << std::endl << std::endl;
#endif // TRACE
#endif // MATRIX
	}
#ifdef MATRIX
	bool Intersect( const Ray &_r, Intersection &_i ) const
	{
        // the incoming Ray and Intersection are in World coordinates
        // Create local ones in Object coordinates
        Ray r;
        r.origin = worldToObject * glm::vec4(_r.origin, 1.0); // position, w = 1
        r.direction = normalToObject * _r.direction; // dir, w = 0, scaled, so don't normalize?
        printvec( "o", r.origin );
        printvec( "d", r.direction );
        std::cout << std::endl;
        printvec( "_o", _r.origin );
		printvec( "_d", _r.direction );
        std::cout << std::endl;
        Intersection i;

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

        // UPDATE 2019-9-24: When doing this with matrix transformations,
        // simply treat center as the origin.
		// glm::vec3 oc = r.origin;
		// float DdotOC = glm::dot( r.direction, oc );
		// float len2 = glm::dot(oc, oc);

		//std::cout << DdotOC << std::endl;

        float DdotD = glm::dot( r.direction, r.direction );
        float DdotO = glm::dot( r.direction, r.origin );
        float OdotO = glm::dot( r.origin, r.origin );
        float a = DdotD;
		float b = DdotO;
		float c = (OdotO - radius2)*a;

		float b2 = b * b;

		// No intersect if miss or tangent
		if( b2 <= c || a == 0.0 )
		{
			_i.distance = 1e9;
			_i.gothit = false;
			return false;
		}

		// two intersection points; choose smallest positive one
		float discriminant = std::sqrt( b2 - c );
		float root1 = (-b + discriminant) / a;
		float root2 = (-b - discriminant) / a;

		i.distance = std::min(root1, root2);
		if( i.distance <= 0.0f )
		{
			// inside the sphere; for now don't intersect
			// but later could use reversed normals.
			_i.distance = 1e9;
			_i.gothit = false;
			return false;
		}

		glm::vec3 dt = r.direction * i.distance;
        i.position = r.origin + dt;

		//i.object = reinterpret_cast<const Object *>(this);
		_i.oid = oid;
		_i.position = glm::vec3(objectToWorld * glm::vec4(i.position, 1.0));//_r.origin + dt;
		// Note: o + dt - c = (o - c) + dt
		//i.normal = glm::normalize(oc + dt);
		_i.normal = glm::normalize(glm::vec3(i.position));
		_i.gothit = true;

        if( _r.origin.x != 0.0 )
        {
        printvec( "_p", _i.position );
        printvec( "p", i.position );
		printvec( "_n", _i.normal );
        std::cout << " " << i.distance << " ";
        }

		// TODO: texture coordinates
		/*
		 * 	u = sqrt( normal[0]*normal[0] + normal[2]*normal[2] ) * 2. # multiply by 2 because X goes 360 where Y goes 180
			v = arctan2(normal[2],normal[0])
			texture = array([u,v])
		 */

        if( _r.origin.x != 0.0 )
        {
		std::cout << "done with Intersect" << std::endl;
        }
		return true;
	}
#else // !MATRIX, use vector intersection
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
#endif // MATRIX
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
#endif /* OBJECT_H_ */
