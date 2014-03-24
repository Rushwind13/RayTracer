/*
 * Lighting.h
 *
 *  Created on: Mar 23, 2014
 *      Author: jimbo
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_

class Lighting
{
public:
	Color Diffuse();
	Color Specular();
	Color Emissive();
	Color Ambient();

protected:
	Color Phong();
	Color PhongBlinn();

};



#endif /* LIGHTING_H_ */
