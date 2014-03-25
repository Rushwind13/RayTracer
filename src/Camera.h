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

	Camera(Position _eye, Position _lookAt, vec3 _up, double _fovy, double width, double height):
		eye(_eye), lookAt(_lookAt), up(_up), fovy(_fovy)
	{
		CreateCoordinateFrame();
		double angle = fovy * deg2rad;
		double aspect_ratio = width / height;
		double tan = tan(angle*0.5);
		double half_height = height * 0.5;
		double half_width = width * 0.5;

	}
	double angle;
	double aspect_ratio;
	double tan;
	double half_height;
	double half_width;

	Ray RayThroughPoint( double i, double j, )
	{
		double b = tan * (half_height - i) / half_height;
		double a = tan * aspect_ratio * (j - half_width) / half_width;

		vec3 origin = eye;
		vec3 direction = normalize(a*u - b*v - w);
		return Ray( origin, direction );
	}

	Camera(){};
	~Camera(){};

	/**
	 * def create_camera( metadata ):
	camera = {}
	camera['eye'] = array(metadata['camera']['pOrigin'])
	camera['center'] = array(metadata['camera']['pLookAt'])
	camera['up'] = array(metadata['camera']['nUp'])
	camera['fovy'] = metadata['camera']['fov']
	w = metadata['world']['size']['w']
	h = metadata['world']['size']['h']
	fovy = deg2rad(metadata['camera']['fov']) ### should be fov/2
	fovx = fovy# * ( w/h )) ### precalc aspect ratio
	camera['fovy'] = fovy
	camera['fovx'] = fovx
	return camera
	 * def CreateCoordinateFrame( camera ):
	a = subtract( camera['eye'], camera['center'] )
	b = camera['up']

	#w = a/|a|
	w = a / la.norm(a)

	#u = b x w / |b x w|
	u = cross( b, w )
	u = u / la.norm(u)

	#v = w x u
	v = cross( w, u )

	return array([u,v,w])

def RayThroughPoint( camera, uvw, w,h, i,j ):
	global camera_dist
	origin = camera['eye'] - camera['center'] ### 3/2014 BUG -- this should be just camera['eye']
	camera_dist = la.norm(origin) ### not needed
	#origin = origin / camera_dist  ## not needed
	fovy = camera['fovy'] ### deg2rad?
	fovx = camera['fovx'] ### calc via aspect ratio
	alpha = math.tan(fovx) * ((j - (w/2.0))/(w/2.0) )* w/h
	beta = math.tan(fovy) *((i - (h/2.0))/(h/2.0) )# * h/w
	#print array([alpha,beta])

	direction = alpha * uvw[0] - beta * uvw[1] - uvw[2]
	direction = direction / la.norm(direction)

	return array([origin,direction])
	 *
	 */
};



#endif /* CAMERA_H_ */
