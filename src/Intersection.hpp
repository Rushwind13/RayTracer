/*
 * Intersection.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: jiharris
 */

#ifndef INTERSECTION_HPP_
#define INTERSECTION_HPP_
#include "Math.hpp"

class Object;

class Intersection
{
public:
	Intersection( float _distance = 1e9 ) : gothit(false), anyhit(false), object(NULL), oid(-1), normal(0), position(0), distance(_distance)
	{
		// For shadow rays, pass in the distance to the light, and then take any object hit, not just the nearest one.
		if( distance < 1e9 )
		{
			bool anyHit = true;
		}
	};
	~Intersection() {};
	//float x,y; // pixel coordinates
	//Ray rTest; // ray that's being tested
	bool gothit; //.
	bool anyhit;
	const Object *object; //.
	short oid; //.
	vec3 normal; //.
	Position position; //.
	float distance; //.
	// TODO: vec2 texture coordinates;
};



#endif /* INTERSECTION_HPP_ */
