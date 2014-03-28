/*
 * Intersection.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: jiharris
 */

#ifndef INTERSECTION_HPP_
#define INTERSECTION_HPP_
#include "Math.hpp"

class JObject;

class Intersection
{
public:
	Intersection() : gothit(false), object(NULL), normal(0), position(0), distance(1e9) {};
	~Intersection() {};
	bool gothit;
	const JObject *object;
	vec3 normal;
	Position position;
	float distance;
	// TODO: vec2 texture coordinates;
};



#endif /* INTERSECTION_HPP_ */
