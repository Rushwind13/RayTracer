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
	int object_count;
	int light_count;

protected:
private:

public:
	World() : refractiveindex(1.0), maxdepth(4), minweight(0.1){ setup(); };
	~World(){objects.clear(); lights.clear();};

	void setup()
	{
		sprintf( filename, "test.png" );

		//	create world object list
    glm::vec3 center(-2.5,0.0,-5.0);
		float radius=2.0;
		Sphere *sphere = new Sphere(center, radius);
		sphere->oid = 1;
		sphere->name = "sphere1";

		objects.push_back(sphere);

		glm::vec3 center2(2.5,0.0,-10.0);
		float radius2=4.0;
		Sphere *sphere2 = new Sphere(center2, radius2);
		sphere2->color = Color(0.1,0.1,1.0);
		sphere2->oid = 2;
		sphere2->name = "sphere2";

		objects.push_back(sphere2);

		glm::vec3 center3(0.0,8.0,-20.0);
		float radius3=4.0;
		Sphere *sphere3 = new Sphere(center3, radius3);
		sphere3->color = Color(0.1,1.0,0.1);
		sphere3->oid = 3;
		sphere3->name = "sphere3";

		objects.push_back(sphere3);/**/

		object_count = objects.size();

		// create world light list
		Position lpos0(0.0,5.0, -1.0);
		Light *light0 = new Light(lpos0);
		light0->color = Color(1.0,1.0,1.0);
		light0->oid = 0;
		light0->name = "light0";

		lights.push_back(light0);

		/*Position lpos1(2.5, 8.0, -10.0);
		Light *light1 = new Light(lpos1);
		light1->color = Color(1.0,1.0,1.0);
		light1->oid = 1;
		light1->name = "light1";

		lights.push_back(light1);/**/

		light_count = lights.size();
	}

	Object *FindObject( const std::string object_name )
	{
		Object *retval = NULL;

		for( std::list<Object *>::iterator it = objects.begin(); it != objects.end(); it++ )
		{
				Object *obj = *it;
				std::cout << object_name << ":" << obj->name << std::endl;
				if( object_name.compare(obj->name) == 0 )
				{
					retval = obj;
					break;
				}
		}

		return retval;
	}

	Object *FindObject( const short oid )
	{
		Object *retval = NULL;

		for( std::list<Object *>::iterator it = objects.begin(); it != objects.end(); it++ )
		{
				// actually, first create the vector to the light, and take N.L to see if light is below surface
				// shouldn't it sometimes be below the surface during refraction? hmm.
				Object *obj = *it;
				if( oid == obj->oid )
				{
					retval = obj;
					break;
				}
		}

		return retval;
	}

	Light *FindLight( const short lid )
	{
		Light *retval = NULL;

		for( std::list<Light *>::iterator it = lights.begin(); it != lights.end(); it++ )
		{
				// actually, first create the vector to the light, and take N.L to see if light is below surface
				// shouldn't it sometimes be below the surface during refraction? hmm.
				Light *obj = (Light *)*it;
				if( lid == obj->oid )
				{
					retval = obj;
					break;
				}
		}

		return retval;
	}
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
					nearest.oid			= i.oid;
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
