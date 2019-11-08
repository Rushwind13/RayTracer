/*
 * Math.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef MATH_H_
#define MATH_H_
#include <msgpack.hpp>
#include <cmath>

#include "glm/glm.hpp"
//using namespace glm;
/*typedef glm::mat3 mat3 ;
 typedef glm::mat4 mat4 ;
 typedef glm::vec3 vec3 ;
 typedef glm::vec4 vec4 ;/**/
const float tau = 6.283185307179586;
const float deg2rad = tau / 360.0;

typedef glm::vec3 Color;
typedef glm::vec3 Position;
// class Position : public glm::vec4
// {
// public:
//     Position(float _x, float _y, float _z){ x=_x; y=_y; z=_z; w=1.0; };
//     Position(glm::vec3 v){ x=v.x; y=v.y; z=v.z; w=1.0; };
//     Position(glm::vec4 v){ x=v.x; y=v.y; z=v.z; w=1.0; };
//     Position(){ x=y=z=0.0; w=1.0; };
//     Position(float _s){ x=y=z=_s; w=1.0; };
//     Position(const Position &p) {x=p.x; y=p.y; z=p.z; w=1.0; };
// };

void printvec( const std::string label, const glm::vec3 vec )
{
	std::cout << label << ": " << vec.x << " " << vec.y << " " << vec.z << "  ";
}

glm::vec3 ReflectVector(const glm::vec3 vIncident, const glm::vec3 vNormal) {
	// vR = vI - [2 * (N . I)]N
	glm::vec3 vReflected;

	float NScalar = 2.0 * glm::dot(vIncident, vNormal);
	glm::vec3 vNscaled = vNormal * NScalar;
	vReflected = glm::normalize(vNscaled - vIncident);

	return vReflected;
}

glm::vec3 RefractVector(const glm::vec3 vIncident, const glm::vec3 vNormal, const float n1,
		const float n2) {
	// vT = (n1/n2)vI - [cosThetat * (n1/n2)(N . I)]N
	// cosThetat = angle between transmitted ray and -N
	float eta_ratio = n1 / n2; // comparison between indices of refraction
	float IdotN = glm::dot(vIncident, vNormal);

	// man, what a mess. root of 1 - [ ratio^2 * ( 1 - (dotprod^2) ) ]
	float ct1 = eta_ratio * eta_ratio;
	float ct2 = 1.0 - (IdotN * IdotN);
	if (ct1 * ct2 > 1.0)
		return glm::vec3(0.0,0.0,0.0); // sqrt undefined -- total internal reflection
	float cosThetat = std::sqrt(1.0 - (ct1 * ct2));
	float Nscalar = cosThetat + (eta_ratio * IdotN);
	glm::vec3 vIscaled = vIncident * eta_ratio;
	glm::vec3 vNscaled = vNormal * Nscalar;

	glm::vec3 vRefracted = glm::normalize(vIscaled - vNscaled);

	return vRefracted;
}

inline float lerp(const float point, const float min, const float max)
{
	return point / (max - min);
}

#define MPACK(vec) vec.x, vec.y, vec.z

class Ray {
public:
	MSGPACK_DEFINE( MPACK(direction), MPACK(origin), length )
	glm::vec3 direction;
	Position origin;
	float length;

	Ray(Position o, glm::vec3 d) :
			direction(d), origin(o) {
		length = direction.length();
		direction = glm::normalize(direction);
	}

	Ray(Position o, glm::vec3 d, float l) :
			direction(d), origin(o), length(l) {
		direction = glm::normalize(direction);
	}

	Ray() {};
};

#endif /* MATH_H_ */
