/*
 * Lighting.h
 *
 *  Created on: Mar 23, 2014
 *      Author: jimbo
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_
#include "Math.hpp"

class Lighting
{
public:
	Lighting() {}
	~Lighting() {}

	// Vector from a given intersection point to a given light.
	static inline void vL( Light light, Intersection hit, glm::vec3 &vL, float &light_dist )
	{
		// Returns a normalized vector from the hit point to the light
		vL = (light.position) - (hit.position);
		light_dist = glm::length(vL);
		vL = vL / light_dist;
	}

	void Shadow( glm::vec3 vL, Intersection hit, Ray *ray )
	{
		// create a ray
		// where o = hit.position and d = a normalized vector pointing at the light;
		ray->origin = hit.position;
		ray->direction = vL;

		// normalize d but take the length first, light_dist
		// do an Intersect run, but only take the first object closer than light_dist
		// if an intersect is found, shadow returns true (and no other lighting calls get made for this light/hit)
		// if no intersect, then calls get made to diffuse / specular / reflection / refraction calculations for this light/hit.
	}
	Color Diffuse();
	Color Specular();
	Color Emissive();
	Color Ambient();

protected:
	Color Phong();
	Color PhongBlinn();
};



#endif /* LIGHTING_H_ */
