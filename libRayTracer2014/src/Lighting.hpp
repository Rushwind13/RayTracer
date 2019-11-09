/*
 * Lighting.h
 *
 *  Created on: Mar 23, 2014
 *      Author: jimbo
 */

#ifndef LIGHTING_H_
#define LIGHTING_H_
#include "Math.hpp"

class Lighting
{
public:
	Lighting() {}
	~Lighting() {}

	// Vector from a given intersection point to a given light.
	static inline void vL( Light light, Intersection hit, Direction &vL, float &light_dist )
	{
		// Returns a normalized vector from the hit point to the light
		vL = (light.position) - (hit.position);
		light_dist = glm::length((glm::vec4)vL);
		vL = vL / light_dist;
	}

	void Shadow( Direction vL, Intersection hit, Ray *ray )
	{
		// create a ray
		// where o = hit.position and d = a normalized vector pointing at the light;
		ray->origin = hit.position;
		ray->direction = vL;

		// normalize d but take the length first, light_dist
		// do an Intersect run, but only take the first object closer than light_dist
		// if an intersect is found, shadow returns true (and no other lighting calls get made for this light/hit)
		// if no intersect, then calls get made to diffuse / specular / reflection / refraction calculations for this light/hit.
	}
	Color Diffuse();
	Color Specular();
	Color Emissive();
	Color Ambient();
	/**
	 *
def Lighting( ray, depth, weight, n1, hit, scene ):
	# I = A + E + sum(i, Vi * (Li / (c0+c1r+c2r^2)) * ( D * max(N . L, 0) + S * max(N . H, 0)^s)
	# I = final intensity
	# A = ambient
	# E = self-emission
	# D = diffuse (summed over all lights i)
	# S = specular (summed over all lights i)
	# Li = intensity of light i
	# vN = surface normal
	# vL = direction to light
	# vH = half-angle (?)
	# s = shininess
	# Vi = visibility to light i (raytracers only); for shadows
	# c0..c2 = attenuation (set to 1,0,0 for no attenuation or 0,0,1, like a physical point light)
	material = hit['object']['material']

	A = array([0.,0.,0.])
	E = array([0.,0.,0.])
	D = array([0.,0.,0.])
	S = array([0.,0.,0.])
	vN = hit['normal']
	texture = False
	if 'ambient' in material:	A = array(material['ambient'])
	if 'emissive' in material:	E = array(material['emissive'])
	if 'diffuse' in material:	D = array(material['diffuse'])
	if 'reflection' in material:	S = array(material['reflection'])
	if 'texture' in material: texture = material['texture']
	s = 100.

	# using the hit direction as the "eye" direction -- I don't think this is right
	vHit = hit['intersection']
	vHit = vHit
	#print vHit
	pEye = ray[0] #scene['eye']
	vDir = ray[1]
	#vEye = vEye - vHit
	#vEye = vEye / la.norm(vEye)
	#print vEye
	#vEye = vEye

	phong = True
	phong_blinn = False
	gloss = phong or phong_blinn
	ambient = True
	emissive = True
	lambert = True
	shadow = True
	reflection = True
	refraction = False
	recursion = reflection or refraction
	fog = False

	if refraction:
		if n1 == scene['refractiveindex']:
			# transition from scene into an object
			transparency = material['refraction']['t']
			n2 = material['refraction']['n']
			Wt = 1. - transparency
		else:
			# transition from "inside" an object back into the scene
			transparency = 0.
			n2 = scene['refractiveindex']
			vN = -vN
			Wt = 1.
	else:
		Wt = 1.
		transparency = 0.
		n2 = n1

	#if not reflection:
	Wr = 1.
	reflectivity = 0.

	###
	# AMBIENT & EMISSIVE
	###
	I = array([0.,0.,0.])
	if ambient: I = I + A
	if emissive and (E != array([0.,0.,0.])).any(): I = I + E
	else:
		# Now that you have the background lights on, loop through the
		# world lights to determine 3D shading
		for light in scene['lights']:
			# is the "direction to the light" relative to the hit point, or just its position in space?
			vL = array(scene['lights'][light]['position']) - vHit
			#vL = scene['lights'][light]['position']
			light_dist = la.norm(vL)
			vL = vL / light_dist

			# first, check to see whether this light can possibly affect this point
			NdotL = dot(vN,vL)
			if NdotL < 0.: continue # cos < 0 means light coming from below surface.

			if shadow:
				# cast shadow ray
				vOrigin = deepcopy(vHit)
				vDir = deepcopy(vL)
				rShadow = array([vOrigin, vDir, vN])
				shadowed = Shadow( rShadow, scene, hit['object'], hit, light_dist )
				if shadowed:
					#print "shadowed"
					continue

			# Not shadowed, so calculate the diffuse and specular components
			if 'color' in scene['lights'][light]: Li= array(scene['lights'][light]['color'])
			else: Li = array([0.,0.,0.])

			if fog:
				#print light_dist
				fog_falloff = 1. / (light_dist)# * light_dist)
				#print fog
				Li = Li * fog_falloff
				#print Li
				#print

			###
			# DIFFUSE, matte
			###
			if lambert:
				if texture: tex = checkerboard( hit['texture'] )
				else: tex = 1.
				diffuse = D * tex * NdotL # "cos falloff", scale D between 0..D
			else:
				diffuse = array([0.,0.,0.])

			###
			# SPECULAR, glossy
			###

			# gloss based on reflected . eye
			if gloss:
				#vViewer = pEye - vHit
				vViewer = vDir
				if phong:
					vR = ReflectVector( vL, vN )
					cosTheta = dot( vR, vViewer ) # Should this be vHit instead?
					#print cosTheta
					if cosTheta < 0.: cosTheta = 0.
				# gloss based on half-angle . normal
				elif phong_blinn:
					vH = vL + vViewer
					len = la.norm(vH)
					if len != 0. :
						vH = vH / len
						cosTheta = dot( vN, vH )
						#print cosTheta
						if cosTheta < 0. : cosTheta = 0.
					else:
						# Not sure what would make len = 0 but if it does, no gloss for you
						cosTheta = 0.

				specular = S * pow( cosTheta, s ) # similar cos falloff, but use s for specular spot size
				if reflection:
					reflectivity = S[0] # for now, tie "reflection" to the specular color (the idea being that more specular stuff reflects more)
					Wr = 1. - reflectivity
					Li = Li * Wr # ...and reflective surfaces don't have their own color, just the stuff that's reflected
			# or turn specular off
			else:
				specular = array([0.,0.,0.])

			# Add the contribution from this light
			#print diffuse
			#print specular
			#print Li
			#print light
			#print depth
			#print hit
			#print
			I = I + (Li * (diffuse + specular))
		I = I * weight * Wr * Wt # modify local color for reflection and transparency
		# TODO: Now that you've got the local color, do any reflection and refraction
		if( recursion and depth + 1 < scene['maxdepth'] ):
			minweight = scene['minweight']
			if( reflection and reflectivity*weight > minweight ):
				#distance = la.norm(vHit)
				#vHit = vHit / distance
				vR = ReflectVector( vDir, vN )
				#print vDir
				#print vN
				#print
				#vR = vR * distance
				rcolor = Trace( depth+1, weight * reflectivity, n2, array([vHit,vR]), scene)
				I = I + reflectivity * rcolor
				#print "rcolor: " + array_str(rcolor, suppress_small=True)
			if( refraction and transparency*weight > minweight ):
				#distance = la.norm(vHit)
				#vHit = vHit / distance
				vT = RefractVector(vDir, vN, n1, n2)
				if (vT != array([0.,0.,0.])).any():
					tcolor = Trace( depth+1, weight * transparency, n2, array([vHit,vT]), scene)
					I = I + transparency * tcolor
					#print tcolor
				else:
					#print "got here"
					I = array([1.,0.,1.])
			#print I
			#print '\n'

	if depth == 0:
		#print '\n'
		# Now that we've looped through all the lights,
		# we need to clamp the color to 0..255 in each channel
		#print array_str(I, precision=1, suppress_small=True)
		clip( I, 0., 1., out=I)
		I = I * 255
		#print I

	return I
	 */

protected:
	Color Phong();
	Color PhongBlinn();

	/**
	 * def Shadow( ray, scene, from_object, old_hit, clip_dist ):
	hit = Intersect( ray, scene, from_object, clip_dist )
	#if hit['gothit']:# and from_object['id'] == hit['object']['id']:
	#	print ray
	#	print old_hit
	#	print hit
	#	print '\n'
	#else: print '0'
	return hit['gothit']
	*
	*
	 */

};



#endif /* LIGHTING_H_ */
