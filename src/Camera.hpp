/*
 * Camera.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "Math.hpp"
class Camera
{
public:
	/**
	 * eye = array([0., 0., 5.])
#     center = array([0.,0.,0.])
#     up = array([0.,1.,0.])
#     width = 200.0
#     height = 100.0
#     fovy = 90.0
#     fovx = fovy * (width / height)
#     camera = {'eye': eye, 'center':center, 'up':up, 'fovy':fovy, 'fovx':fovx}
	 */
	Position eye;
	Position lookAt;
	vec3 up;

	// I'm torn about whether the viewport size should be here or somewhere else.
	unsigned int width, height;

	float fovy;

	Camera(Position _eye, Position _lookAt, vec3 _up, float _fovy, float _width, float _height):
		eye(_eye), lookAt(_lookAt), up(_up), fovy(_fovy), width (_width), height(_height)
	{
		CreateCoordinateFrame();
		angle = fovy * deg2rad;
		aspect_ratio = width / height;
		_tan = std::tan(angle*0.5);
		half_height = height * 0.5;
		half_width = width * 0.5;

	}

	Ray RayThroughPoint( float i, float j )
	{
		float b = _tan * (half_height - i) / half_height;
		float a = _tan * aspect_ratio * (j - half_width) / half_width;

		vec3 origin = eye;
		vec3 direction = (a*u) - (b*v) - w;
		return Ray( origin, direction );
	}

	Camera(){};
	~Camera(){};

protected:
	// precalculate as much as possible, so that RayThroughPoint moves faster
	float angle;
	float aspect_ratio;
	float _tan;
	float half_height;
	float half_width;

	// Camera local coordinates
	vec3 u,v,w;

	void CreateCoordinateFrame()
	{
		// Forward vector
		vec3 a = eye - lookAt;
		vec3 b = up;

		// w = a/|a|
		w = normalize(a);

		// Right vector (orthogonal to fwd and up)
		// u = b x w / |b x w|
		u = cross(b,w);
		u = normalize(u);

		// Up vector
		// v = w x u
		v = cross(w,u);
	}
};



#endif /* CAMERA_H_ */
