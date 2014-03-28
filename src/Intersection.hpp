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
	Intersection() : gothit(false), object(NULL), normal(0), position(0), distance(1e9) {};
	~Intersection() {};
	bool gothit;
	const Object *object;
	vec3 normal;
	Position position;
	float distance;
	// TODO: vec2 texture coordinates;
};



#endif /* INTERSECTION_HPP_ */
