/*
 * World.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef WORLD_H_
#define WORLD_H_
#include <list>

#include "Object.hpp"

class World
{
public:
	char filename[255];
	double refractiveindex;
	int maxdepth;
	double minweight;

	std::list<Object *> objects;
	std::list<Light *> lights;

protected:
private:

public:
	World() : refractiveindex(1.0), maxdepth(3), minweight(0.1){};
	~World(){};

	void setup();
/**
 *
def create_scene( metadata ):
	scene = {}
	scene['filename'] = metadata['world']['filename']
	scene['object'] = metadata['objects']
	scene['lights'] = metadata['lights']
	scene['eye'] = array(metadata['camera']['pOrigin']) - array(metadata['camera']['pLookAt'])
	scene['refractiveindex'] = metadata['world']['background']['refraction']['n']
	scene['maxdepth'] = metadata['world']['maxdepth']
	scene['minweight'] = metadata['world']['minweight']

	return scene
 *
 */
	Intersection Intersect( Ray ray, float max_distance = 1e9 )
	{
		Intersection nearest( max_distance );

		for(std::list<Object *>::iterator it = objects.begin(); it != objects.end(); it++)
		{
			Intersection i;
		    if( (*it)->Intersect( ray, i ) )
		    {
				if( i.distance < nearest.distance )
				{
					nearest.gothit		= true;
					nearest.distance	= i.distance;
					nearest.normal		= i.normal;
					nearest.object		= i.object;
					nearest.position	= i.position;
				}
				// For shadow rays, any hit will do (don't need the actual closest one)
				if( nearest.anyhit && nearest.gothit )
				{
					std::cout << nearest.distance << " ";
					return nearest;
				}
		    }
		}

		return nearest;
	};
protected:
private:
};



#endif /* WORLD_H_ */
