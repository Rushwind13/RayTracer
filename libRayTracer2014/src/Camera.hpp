/*
 * Camera.h
 *
 *  Created on: Mar 22, 2014
 *      Author: jimbo
 */

#ifndef CAMERA_H_
#define CAMERA_H_

//#define DEBUG
#define INFO

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
	Direction up;

	// I'm torn about whether the viewport size should be here or somewhere else.
	unsigned int width, height;

	float fovy;

	Camera(Position _eye, Position _lookAt, Direction _up, float _fovy, float _width, float _height):
		eye(_eye), lookAt(_lookAt), up(_up), width (_width), height(_height), fovy(_fovy)
	{
		init();
	}

	void init()
	{

		CreateCoordinateFrame();
		angle = fovy * deg2rad;
		aspect_ratio = (float)width / (float)height;
		//std::cout << (float)width << " " << (float)height << " " << (float)aspect_ratio << std::endl;
		_tan = std::tan(angle*0.5);
		half_height = height * 0.5;
		half_width = width * 0.5;
        printvec("eye", eye);
        std::cout << std::endl;
	}

	void setup()
	{
		//		- create camera
		// TODO: Need to implement a scene file
		Position _eye(0.0,0.0,0.0);
		Position _lookAt(0.0,0.0,-1.0);
		Direction _up(0.0,1.0,0.0);
		float _fovy = 90.0;
		float _width = 150.0;
		float _height = 100.0;/**/
		/*float _width = 600.0;
		float _height = 400.0;/**/
		/*float _width = 1620.0;
		float _height = 1080.0;/**/

		eye = _eye;
		lookAt = _lookAt;
		up = _up;
		fovy = _fovy;
		width = _width;
		height = _height;

		init();
	}
	/*Ray RayThroughPoint( float i, float j )
		{
			float b = _tan * (half_height - i) / half_height;
			float a = _tan * aspect_ratio * (j - half_width) / half_width;

			vec3 origin = eye;
			vec3 direction = (a*u) - (b*v) - w;
			direction = normalize(direction);
	#ifdef DEBUG
			printvec("o", origin);
			printvec("d", direction);
			std::cout << std::endl;
	#endif

			return Ray( origin, direction );
		}/**/
	Ray RayThroughPoint( float i, float j )
		{
			float a = ((2.0 * (i + 0.5) / width) - 1.0) * _tan * aspect_ratio;
			float b = (1.0 - (2.0 * (j + 0.5 )) / height) * _tan;

			Position origin = eye;
			Direction direction = (a*u) - (b*v) - w;
			direction = glm::normalize(direction);
	#ifdef DEBUG
			std::cout << "(" << i << "," << j << ") ";
			printvec("o", origin);
			printvec("d", direction);
			std::cout << std::endl;
	#endif

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
	glm::vec3 u,v,w;

	void CreateCoordinateFrame()
	{
		// Forward vector
		glm::vec3 a = eye - lookAt;
		glm::vec3 b = up;

		// w = a/|a|
		w = glm::normalize(a);

		// Right vector (orthogonal to fwd and up)
		// u = b x w / |b x w|
		u = glm::cross(b,w);
		u = glm::normalize(u);

		// Up vector
		// v = w x u
		v = glm::cross(w,u);
#ifdef INFO
		printvec("u", u);
		printvec("v", v);
		printvec("w", w);
		std::cout << std::endl;
#endif
	}
};



#endif /* CAMERA_H_ */
