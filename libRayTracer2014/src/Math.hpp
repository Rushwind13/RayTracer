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

const float tau = 6.283185307179586;
const float deg2rad = tau / 360.0;


void printvec( const std::string label, const glm::vec3 vec )
{
	std::cout << label << ": " << vec.x << " " << vec.y << " " << vec.z << "  ";
}
void printvec( const std::string label, const glm::vec4 vec )
{
	std::cout << label << ": " << vec.x << " " << vec.y << " " << vec.z << "  " << vec.w << "  ";
}
void printmat( const std::string label, const glm::mat4 mat )
{
	printvec(label, mat[0]); std::cout << std::endl;
	printvec(label, mat[1]); std::cout << std::endl;
	printvec(label, mat[2]); std::cout << std::endl;
	printvec(label, mat[3]); std::cout << std::endl;
}

class Position : public glm::vec4
{
public:
    Position(float _x, float _y, float _z, float _w=1.0){ x=_x; y=_y; z=_z; w=_w; /*printvec("<-xyz", *(this));/**/};
    Position(glm::vec3 v){ x=v.x; y=v.y; z=v.z; w=1.0; };
    Position(glm::vec4 v){ x=v.x; y=v.y; z=v.z; w=v.w; };
    Position(){ x=y=z=0.0; w=1.0; };
    Position(float _s){ x=y=z=_s; w=1.0; };
    Position(const Position &p) {x=p.x; y=p.y; z=p.z; w=p.w; /*printvec("<-Position", *(this));/**/};
};
class Direction : public glm::vec4
{
public:
    Direction(float _x, float _y, float _z, float _w=0.0){ x=_x; y=_y; z=_z; w=_w; };
    Direction(glm::vec3 v){ x=v.x; y=v.y; z=v.z; w=0.0; };
    Direction(glm::vec4 v){ x=v.x; y=v.y; z=v.z; w=v.w; };
    Direction(){ x=y=z=0.0; w=0.0; };
    Direction(float _s){ x=y=z=_s; w=0.0; };
    Direction(const Direction &d) {x=d.x; y=d.y; z=d.z; w=d.w; };
};
class Color : public glm::vec4
{
public:
    Color(float _r, float _g, float _b, float _a=0.0){ r=_r; g=_g; b=_b; a=_a; };
    Color(glm::vec3 v){ r=v.r; g=v.g; b=v.b; a=0.0; };
    Color(glm::vec4 v){ r=v.r; g=v.g; b=v.b; a=v.a; };
    Color(){ r=g=b=0.0; a=0.0; };
    Color(float _s){ r=g=b=_s; a=0.0; };
    Color(const Color &c){ r=c.r; g=c.g; b=c.b; w=c.a;/*printvec("<-Color", *(this));/**/};
};

Direction ReflectVector(const Direction vIncident, const Direction vNormal) {
	// vR = vI - [2 * (N . I)]N
	Direction vReflected;

	float NScalar = 2.0 * glm::dot((glm::vec4)vIncident, (glm::vec4)vNormal);
	Direction vNscaled = vNormal * NScalar;
	vReflected = glm::normalize(vNscaled - vIncident);

	return vReflected;
}

Direction RefractVector(const Direction vIncident, const Direction vNormal, const float n1,
		const float n2) {
	// vT = (n1/n2)vI - [cosThetat * (n1/n2)(N . I)]N
	// cosThetat = angle between transmitted ray and -N
	float eta_ratio = n1 / n2; // comparison between indices of refraction
	float IdotN = glm::dot((glm::vec4)vIncident, (glm::vec4)vNormal);

	// man, what a mess. root of 1 - [ ratio^2 * ( 1 - (dotprod^2) ) ]
	float ct1 = eta_ratio * eta_ratio;
	float ct2 = 1.0 - (IdotN * IdotN);
	if (ct1 * ct2 > 1.0)
		return Direction(0.0,0.0,0.0); // sqrt undefined -- total internal reflection
	float cosThetat = std::sqrt(1.0 - (ct1 * ct2));
	float Nscalar = cosThetat + (eta_ratio * IdotN);
	Direction vIscaled = vIncident * eta_ratio;
	Direction vNscaled = vNormal * Nscalar;

	Direction vRefracted = glm::normalize(vIscaled - vNscaled);

	return vRefracted;
}

glm::mat4 TranslateMatrix( const Position translate )
{
	glm::mat4 result(1.0);
	result[3] = (glm::vec4)translate;

	return result;
}

glm::mat4 ScaleMatrix( const Position translate )
{
	glm::mat4 result(1.0);
	result[0][0] = translate.x;
	result[1][1] = translate.y;
	result[2][2] = translate.z;
	result[3][3] = translate.w;

	return result;
}

glm::mat4 RotateMatrix( const float degrees, const char axis )
{
	glm::mat4 result(1.0);
	float angle = degrees * deg2rad;
  float sin = glm::sin(angle);
  float cos = glm::cos(angle);

	switch(axis)
	{
		case 'X':
			result[1][1] = cos;
			result[2][1] = -sin;
			result[1][2] = sin;
			result[2][2] = cos;
			break;
		case 'Y':
			result[0][0] = cos;
			result[2][0] = sin;
			result[0][2] = -sin;
			result[2][2] = cos;
			break;
		case 'Z':
			result[0][0] = cos;
			result[1][0] = -sin;
			result[0][1] = sin;
			result[1][1] = cos;
			break;
	}

	return result;
}

inline float lerp(const float point, const float min, const float max)
{
	return point / (max - min);
}

#define MPACK(vec) vec.x, vec.y, vec.z, vec.w

class Ray {
public:
	MSGPACK_DEFINE( MPACK(direction), MPACK(origin), length )
	Direction direction;
	Position origin;
	float length;

	Ray(Position o, Direction d) :
			direction(d), origin(o) {
		length = glm::length((glm::vec4)direction);
		direction = glm::normalize(direction);
	}

	Ray(Position o, Direction d, float l) :
			direction(d), origin(o), length(l) {
		direction = glm::normalize(direction);
	}

	Ray() {};
};

#endif /* MATH_H_ */
