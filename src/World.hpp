/*
 * World.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef WORLD_H_
#define WORLD_H_
#include <list>
using namespace std;
#include "Object.hpp"

class World
{
public:
	char filename[255];
	double refractiveindex;
	int maxdepth;
	double minweight;

	//list<Object> objects;

protected:
private:

public:
	World(){};
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
protected:
private:
};



#endif /* WORLD_H_ */
