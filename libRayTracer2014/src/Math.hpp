/*
 * Math.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef MATH_H_
#define MATH_H_
#include <msgpack.hpp>

#include "glm/glm.hpp"

const float tau = 6.283185307179586;
const float deg2rad = tau / 360.0;
const float epsilon = 0.00001;



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
    glm::mat4 flipper = glm::transpose(mat);
	printvec(label, flipper[0]); std::cout << std::endl;
	printvec(label, flipper[1]); std::cout << std::endl;
	printvec(label, flipper[2]); std::cout << std::endl;
	printvec(label, flipper[3]); std::cout << std::endl;
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

glm::mat4 ScaleMatrix( const Position scale )
{
	glm::mat4 result(1.0);
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = scale.z;
	result[3][3] = scale.w;

	return result;
}

// glm::sin and glm::cos are only defiend for 0º-90º
// This function forces all angles to fit within that range,
// with proper sign-flips to put angles in different Quadrants
Direction ClampDegreeRange( const float degrees )
{
	Direction result;
	float angle = degrees;

	if( angle < 0.0 )
	{
		while( angle < 0.0 ) angle += 360.0;
	}

	if( angle > 360.0 )
	{
		while( angle > 360.0 ) angle -= 360.0;
	}

	if( angle > 90.0 && angle <= 180.0 )
	{
		// std::cout << " QII ";
		angle -= 90.0;
		result = Direction(-1.0, 1.0, (90.0 - angle));
	}
	else if( angle > 180.0 && angle <= 270.0 )
	{
		// std::cout << " QIII ";
		angle -= 180.0;
		result = Direction(-1.0, -1.0, angle);
	}
	else if( angle > 270.0 )
	{
		// std::cout << " QIV ";
		angle -= 270.0;
		result = Direction(1.0, -1.0, (90.0 - angle));
	}
	else
	{
		// std::cout << " QI ";
		result = Direction(1.0, 1.0, angle);
	}

	return result;
}

glm::mat4 RotateMatrix( const Direction axis, const float degrees )
{
	glm::mat4 result(1.0);
	Direction _degrees = ClampDegreeRange(degrees);
	float angle = _degrees.z * deg2rad;
	float _cos = _degrees.x * glm::cos(angle);
	float _sin = _degrees.y * glm::sin(angle);

  if( glm::abs(_sin - 0.0) < epsilon ) _sin = 0.0;
  if( glm::abs(_cos - 0.0) < epsilon ) _cos = 0.0;

	glm::mat4 term1 = ScaleMatrix(Position(_cos));
	glm::mat4 term2;
	glm::mat4 term3;

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;
	float xy = x*y;
	float xz = x*z;
	float yz = y*z;

	term2[0] = Direction(x2, xy, xz);
	term2[1] = Direction(xy, y2, yz);
	term2[2] = Direction(xz, yz, z2);
	term2[3] = Direction(0.0);

	term3[0] = Direction( 0,  z, -y);
	term3[1] = Direction(-z,  0,  x);
	term3[2] = Direction( y, -x,  0);
	term3[3] = Direction(0.0);

	// term1 *= cos;
	term2 *= 1.0f - _cos;
	term3 *= _sin;

	result = term1 + term2 + term3;
	result[3] = Position(0.0);

	// printmat("R(a,theta)", result);

	return result;
}

glm::mat4 ShearMatrix(
    const float xy,
    const float xz,
    const float yx,
    const float yz,
    const float zx,
    const float zy )
{
	glm::mat4 result(1.0);
	result[1][0] = xy;
	result[2][0] = xz;
	result[0][1] = yx;
	result[2][1] = yz;
	result[0][2] = zx;
	result[1][2] = zy;

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

    Position apply() { return origin + (direction * length); };
    Position apply( const float t ) const { return origin + (direction * t); };
};

Ray TransformRay( const Ray &in, const glm::mat4 transform )
{
    Ray result;
    result.origin = transform * in.origin;
    result.direction = transform * in.direction;

    // printvec("in", in.origin);
    // std::cout << std::endl;
    // printmat("M", transform);
    // printvec("out", result.origin);
    // std::cout << std::endl;
    // std::cout << std::endl;

    return result;
}
#endif /* MATH_H_ */
