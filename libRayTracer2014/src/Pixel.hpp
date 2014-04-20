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
	Ray primaryRay; // This is the original primary ray, to be kept through recursion, etc. for proper specular highlight direction.

	// Transient variables -- all can be considered "the current working set" for this step
	IntersectType type;
	Ray r; // ray (primary, shadow, reflection, ...)

	// Added from Intersection object (to hold onto during Shadow tests)
	short oid; // object ID (all widgets will have same world data)
	vec3 normal; // Normal at hit point
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
};



#endif /* PIXEL_HPP_ */
