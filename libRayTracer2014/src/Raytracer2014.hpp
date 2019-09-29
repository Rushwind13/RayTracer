/*
 * Raytracer2014.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef RAYTRACER2014_H_
#define RAYTRACER2014_H_

void SaveImage( std::list<Color> image, const World world, const Camera camera );
Color Trace( Ray ray, World world, int depth, float weight, float refractiveindex);
Color FindColor( Ray ray, World world, Intersection hit, int depth, float weight, float refractiveindex );
void raytracer( Camera camera, World world );

#endif /* RAYTRACER2014_H_ */
