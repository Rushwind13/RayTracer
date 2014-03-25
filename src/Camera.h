/*
 * Camera.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "Math.h"
class Camera
{
public:
	Position eye;
	Position lookAt;
	vec3 up;

	vec3 u,v,w;

	double fovy;

	void CreateCoordinateFrame()
	{
		vec3 a = eye - lookAt;
		vec3 b = up;

		w = normalize(a);
		u = cross(b,w);
		u = normalize(u);
		v = cross(w,u);
	}

	Camera(Position _eye, Position _lookAt, vec3 _up): eye(_eye), lookAt(_lookAt), up(_up)
	{
		CreateCoordinateFrame();
	}

	Camera(){};
	~Camera(){};
};



#endif /* CAMERA_H_ */
