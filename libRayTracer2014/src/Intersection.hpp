/*
 * Intersection.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: jiharris
 */

#ifndef INTERSECTION_HPP_
#define INTERSECTION_HPP_
#include <sstream>
#include <msgpack.hpp>
#include "Math.hpp"

class Object;

class Intersection
{
public:
	Intersection( float _distance = 1e9 ) : gothit(false), anyhit(false), oid(-1), normal(0.0), position(0.0)
	{
        distance[0] = distance[1] = _distance;
		// For shadow rays, pass in the distance to the light, and then take any object hit, not just the nearest one.
		if( distance[0] < 1e9 )
		{
			bool anyHit = true;
		}
	};
	~Intersection() {};
	MSGPACK_DEFINE( gothit, anyhit, oid, distance, MPACK(normal), MPACK(position));


	//float x,y; // pixel coordinates
	//Ray rTest; // ray that's being tested
	bool gothit; //.
	bool anyhit; // two types of tests; Shadow tests can early-out on any hit
	short oid; // Object ID of hit Object
	Direction normal; //.
	Position position; //.
	float distance[2]; // t-values for "entry" and "exit" points
	// TODO: vec2 texture coordinates;
};
//friend std::ostream& operator<<(std::ostream& os, const Intersection& i);


//std::ostream& operator<<(std::ostream& os, const Intersection& i)
void PrintIntersection(std::ostream& os, const Intersection& i)
{
    os << i.gothit << ',';
    os << i.anyhit << ',';
    os << i.oid << ',';
    os << i.normal.x << ',' << i.normal.y << ',' << i.normal.z << ',' << i.normal.w << ',';
    os << i.position.x << ',' << i.position.y << ',' << i.position.z << ',' << i.position.w << ',';
    os << i.distance[0] << ',';
    os << i.distance[1] << std::endl;
}

void ReadIntersection(std::string in, Intersection &i)
{
    if(in.length() == 0) return;
    std::istringstream stream(in);

    std::string temp;
    std::getline(stream, temp, ','); i.gothit = stol(temp);
    std::getline(stream, temp, ','); i.anyhit = stol(temp);
    std::getline(stream, temp, ','); i.oid = stol(temp);
    std::getline(stream, temp, ','); i.normal.x = stof(temp);
    std::getline(stream, temp, ','); i.normal.y = stof(temp);
    std::getline(stream, temp, ','); i.normal.z = stof(temp);
    std::getline(stream, temp, ','); i.normal.w = stof(temp);
    std::getline(stream, temp, ','); i.position.x = stof(temp);
    std::getline(stream, temp, ','); i.position.y = stof(temp);
    std::getline(stream, temp, ','); i.position.z = stof(temp);
    std::getline(stream, temp, ','); i.position.w = stof(temp);
    std::getline(stream, temp, ','); i.distance[0] = stof(temp);
    std::getline(stream, temp, '\n'); i.distance[1] = stof(temp);
}

#endif /* INTERSECTION_HPP_ */
