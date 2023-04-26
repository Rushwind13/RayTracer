//============================================================================
// Name        : Black.cpp
// Author      : VT
// Version     :
// Copyright   : Copyright 2014 Jimbo S. Harris. All rights reserved.
// Description : Lighting hub, spawns off shadow, reflection, and refraction tests for each successful intersection
//============================================================================

#include <iostream>
using namespace std;
#include "Black.hpp"
#include "Lighting.hpp"

void Black::local_setup()
{
//#define DEBUG
}

// You will get here if a Shadow test registers a hit with an object between the intersection and a particular light
// So this point is shadowed by this light.
bool Black::local_work(msgpack::sbuffer *header, msgpack::sbuffer *payload)
{
	Pixel pixel;
	msgpack::object obj;
	unPackPart( header, &obj );
	obj.convert( pixel );

	pixel.gothit = true;

	Color ambient(0.01,0.01,0.01);// TODO: this comes from the world
	Color emissive(0.0,0.0,0.0); // TODO: this comes from the object
	pixel.color = ambient + emissive;
#ifdef DEBUG
	std::cout << "(" << pixel.x << "," << pixel.y << ")";
	printvec("c", pixel.color);
	std::cout << std::endl;
#endif /* DEBUG */

	header->clear();
	msgpack::pack( header, pixel );
	payload->clear();
	return true; // send an outbound message as a result of local_work()
}

void Black::local_shutdown()
{
	std::cout << "shutting down... ";
}

int main(int argc, char* argv[])
{
	cout << "starting up" << endl;
    if( argc != 6 )
    {
        cout << "please use start.sh to provide proper CLI args" << endl;
        return 1;
    }
	Black blk(argv[1], argv[2], argv[3], argv[4], argv[5]);

	cout << "running" << endl;
	blk.run();

	cout << "shutting down" << endl;
	return 0;
}
