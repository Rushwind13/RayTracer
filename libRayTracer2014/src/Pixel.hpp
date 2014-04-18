/*
 * Pixel.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: jiharris
 */

#ifndef PIXEL_HPP_
#define PIXEL_HPP_
#include "Math.hpp"

enum IntersectType
{
	iPrimary = 0,
	iShadow,
	iReflection,
	iRefraction
};

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
	// Permanent variables
	float x,y;	// screen coordinates (act as key index for this pixel)
	Color color; // The final color as known at this point
	Ray primaryRay; // This is the original primary ray, to be kept through recursion, etc. for proper specular highlight direction.

	// Transient variables -- all can be considered "the current working set" for this step
	IntersectType type;
	Ray r; // ray (primary, shadow, reflection, ...)
	glm::vec3 vN; // surface normal
	short oid; // object ID (all widgets will have same world data)
	short lid; // light ID (all widgets will have same world data)
	float depth; // for recursion cutoff
	float weight; // for recursion cutoff and light attenuation
	Color deltaColor; // current color for recursive
	bool isRecursive; // So that the pub/sub engine can decide where to send things next
};



#endif /* PIXEL_HPP_ */
