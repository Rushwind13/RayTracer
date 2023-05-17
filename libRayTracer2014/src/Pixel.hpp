/*
 * Pixel.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: jiharris
 */

#ifndef PIXEL_HPP_
#define PIXEL_HPP_
#include <iostream>
#include <msgpack.hpp>
#include "Math.hpp"

#define	iInvalid	-1
#define iPrimary	0
#define	iShadow		1
#define	iReflection	2
#define	iRefraction	3

enum ColorType
{
	cLambert = 0,
	cShadow,
	cPhong,
	cReflection,
	cRefraction
};

// This is the message schema for data required to color a single pixel.
// NOTE: It is intended that this object will be sent as a direct binary blob between widgets,
// so no pointers or other thread-specific (or process-specific!) data goes here.
class Pixel
{
public:
	MSGPACK_DEFINE( x, y, primaryRay, r, MPACK(normal), MPACK(position), distance, lid, NdotL, depth, weight, MPACK(color), oid, gothit, type )

	// Permanent variables
	float x;
	float y;	// screen coordinates (act as key index for this pixel)
	Ray primaryRay; // This is the original primary ray, to be kept through recursion, etc. for proper specular highlight direction.

	// Transient variables -- all can be considered "the current working set" for this step
	int64_t type;
	Ray r; // ray (primary, shadow, reflection, ...)

	// Added from Intersection object (to hold onto during Shadow tests)
	short oid; // object ID (all widgets will have same world data)
	Direction normal; // Normal at hit point
	Position position; // intersection point
	float distance; // distance along primary ray to intersection point.

	// Added by Shader
	short lid; // light ID (all widgets will have same world data)
	float NdotL; // angle between current light and surface normal (pass instead of recalculating)

	short depth; // for recursion cutoff (0-MAXDEPTH)
	float weight; // for recursion cutoff and light attenuation 0.0-1.0


	Color color; // The final color as known at this point
	bool gothit; // Gets set to false when the primary ray misses
	//Color deltaColor; // current color for recursive
	//bool isRecursive; // So that the pub/sub engine can decide where to send things next
}__attribute__((packed));
//friend ostream& operator<<(ostream& os, const Pixel& px);


//ostream& operator<<(ostream& os, const Pixel& px)
void PrintPixel(ostream& os, const Pixel& px)
{
    os << px.x << ',' << px.y << ',';
    os << px.primaryRay.direction.x << ',' << px.primaryRay.direction.y << ',' << px.primaryRay.direction.z << ',' << px.primaryRay.direction.w << ',';
    os << px.primaryRay.origin.x << ',' << px.primaryRay.origin.y << ',' << px.primaryRay.origin.z << ',' << px.primaryRay.origin.w << ',';
    os << px.primaryRay.length << ',';
    os << px.type << ',';
    os << px.r.direction.x << ',' << px.r.direction.y << ',' << px.r.direction.z << ',' << px.r.direction.w << ',';
    os << px.r.origin.x << ',' << px.r.origin.y << ',' << px.r.origin.z << ',' << px.r.origin.w << ',';
    os << px.r.length << ',';
    os << px.oid << ',';
    os << px.normal.x << ',' << px.normal.y << ',' << px.normal.z << ',' << px.normal.w << ',';
    os << px.position.x << ',' << px.position.y << ',' << px.position.z << ',' << px.position.w << ',';
    os << px.distance << ',';
    os << px.lid << ',';
    os << px.NdotL << ',';
    os << px.depth << ',';
    os << px.weight << ',';
    os << px.color.r << ',' << px.color.g << ',' << px.color.b << ',' << px.color.a << ',';
    os << px.gothit << std::endl;
}

void ReadPixel(std::string in, Pixel &pixel)
{
    std::istringstream stream(in);

    std::string temp;
    std::getline(stream, temp, ','); pixel.x = stof(temp);
    std::getline(stream, temp, ','); pixel.y = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.direction.x = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.direction.y = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.direction.z = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.direction.w = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.origin.x = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.origin.y = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.origin.z = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.origin.w = stof(temp);
    std::getline(stream, temp, ','); pixel.primaryRay.length = stof(temp);
    std::getline(stream, temp, ','); pixel.type = stol(temp);
    std::getline(stream, temp, ','); pixel.r.direction.x = stof(temp);
    std::getline(stream, temp, ','); pixel.r.direction.y = stof(temp);
    std::getline(stream, temp, ','); pixel.r.direction.z = stof(temp);
    std::getline(stream, temp, ','); pixel.r.direction.w = stof(temp);
    std::getline(stream, temp, ','); pixel.r.origin.x = stof(temp);
    std::getline(stream, temp, ','); pixel.r.origin.y = stof(temp);
    std::getline(stream, temp, ','); pixel.r.origin.z = stof(temp);
    std::getline(stream, temp, ','); pixel.r.origin.w = stof(temp);
    std::getline(stream, temp, ','); pixel.r.length = stof(temp);
    std::getline(stream, temp, ','); pixel.oid = stol(temp);
    std::getline(stream, temp, ','); pixel.normal.x = stof(temp);
    std::getline(stream, temp, ','); pixel.normal.y = stof(temp);
    std::getline(stream, temp, ','); pixel.normal.z = stof(temp);
    std::getline(stream, temp, ','); pixel.normal.w = stof(temp);
    std::getline(stream, temp, ','); pixel.position.x = stof(temp);
    std::getline(stream, temp, ','); pixel.position.y = stof(temp);
    std::getline(stream, temp, ','); pixel.position.z = stof(temp);
    std::getline(stream, temp, ','); pixel.position.w = stof(temp);
    std::getline(stream, temp, ','); pixel.distance = stof(temp);
    std::getline(stream, temp, ','); pixel.lid = stol(temp);
    std::getline(stream, temp, ','); pixel.NdotL = stof(temp);
    std::getline(stream, temp, ','); pixel.depth = stol(temp);
    std::getline(stream, temp, ','); pixel.weight = stof(temp);
    std::getline(stream, temp, ','); pixel.color.r = stof(temp);
    std::getline(stream, temp, ','); pixel.color.g = stof(temp);
    std::getline(stream, temp, ','); pixel.color.b = stof(temp);
    std::getline(stream, temp, ','); pixel.color.a = stof(temp);
    std::getline(stream, temp, '\n'); pixel.gothit = stol(temp);
}


#endif /* PIXEL_HPP_ */
