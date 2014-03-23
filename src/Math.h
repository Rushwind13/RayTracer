/*
 * Raytracer2014.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef RAYTRACER2014_H_
#define RAYTRACER2014_H_

#include "glm/glm.hpp"
using namespace glm;
/*typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ;
typedef glm::vec3 vec3 ;
typedef glm::vec4 vec4 ;/**/
const double tau = 6.283185307179586;
const double deg2rad = tau / 360.0;

typedef vec3 Color;
typedef vec3 Position;

class Ray
{
public:
	vec3 direction;
	Position origin;
	double length;

	Ray( Position o, vec3 d ): direction(d), origin(o)
	{
		length = direction.length();
		normalize( direction );
	}

	Ray( Position o, vec3 d, double l ): direction(d), origin(o), length(l)
	{
		normalize(direction);
	}
};

#endif /* RAYTRACER2014_H_ */
