/*
 * Math.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef MATH_H_
#define MATH_H_
#include <cmath>

#include "glm/glm.hpp"
using namespace glm;
/*typedef glm::mat3 mat3 ;
 typedef glm::mat4 mat4 ;
 typedef glm::vec3 vec3 ;
 typedef glm::vec4 vec4 ;/**/
const float tau = 6.283185307179586;
const float deg2rad = tau / 360.0;

typedef vec3 Color;
typedef vec3 Position;

void printvec( const std::string label, const vec3 vec )
{
	std::cout << label << ": " << vec.x << " " << vec.y << " " << vec.z << "  ";
}

vec3 ReflectVector(const vec3 vIncident, const vec3 vNormal) {
	// vR = vI - [2 * (N . I)]N
	vec3 vReflected;

	float NScalar = 2.0 * dot(vIncident, vNormal);
	vec3 vNscaled = vNormal * NScalar;
	vReflected = normalize(vNscaled - vIncident);

	return vReflected;
}

vec3 RefractVector(const vec3 vIncident, const vec3 vNormal, const float n1,
		const float n2) {
	// vT = (n1/n2)vI - [cosThetat * (n1/n2)(N . I)]N
	// cosThetat = angle between transmitted ray and -N
	float eta_ratio = n1 / n2; // comparison between indices of refraction
	float IdotN = dot(vIncident, vNormal);

	// man, what a mess. root of 1 - [ ratio^2 * ( 1 - (dotprod^2) ) ]
	float ct1 = eta_ratio * eta_ratio;
	float ct2 = 1.0 - (IdotN * IdotN);
	if (ct1 * ct2 > 1.0)
		return vec3(0); // sqrt undefined -- total internal reflection
	float cosThetat = std::sqrt(1.0 - (ct1 * ct2));
	float Nscalar = cosThetat + (eta_ratio * IdotN);
	vec3 vIscaled = vIncident * eta_ratio;
	vec3 vNscaled = vNormal * Nscalar;

	vec3 vRefracted = normalize(vIscaled - vNscaled);

	return vRefracted;
}

inline float lerp(const float point, const float min, const float max)
{
	return point / (max - min);
}

class Ray {
public:
	vec3 direction;
	Position origin;
	float length;

	Ray(Position o, vec3 d) :
			direction(d), origin(o) {
		length = direction.length();
		direction = normalize(direction);
	}

	Ray(Position o, vec3 d, float l) :
			direction(d), origin(o), length(l) {
		direction = normalize(direction);
	}
};

#endif /* MATH_H_ */
