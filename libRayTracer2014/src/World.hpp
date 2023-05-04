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
#include "Loader.hpp"

class World
{
public:
	char filename[255];
	double refractiveindex;
	int maxdepth;
	double minweight;

	std::list<Object *> objects;
	std::list<Light *> lights;
    std::map<std::string, Color> colors;
	int object_count;
	int light_count;
	Pattern *vert, *horz;

protected:
private:

public:
	World() : refractiveindex(1.0), maxdepth(10), minweight(0.1){ setup(); };
	~World(){objects.clear(); lights.clear();};

	void setup()
	{
        load_world();
	}
    void load_world()
    {
        int _oid = 0;
        int _lid = 0;
        std::cout << "loading world...";
        std::ifstream f("../bin/World.json");
        json data = json::parse(f);
        std::string _filename;
        field_from_json(data, "filename", "test.png", _filename);
        std::strcpy(filename, _filename.c_str());

        for( auto &x: data["colors"].items() )
        {
            json _json = x;
            const std::string name = x.key();
            std::cout << name << " loading...";

            float color[3];
            array_from_json(_json, name, 0.0, color);
            colors[name] = Color(color);
        }
        std::cout<< "objects done." << std::endl;
        for( auto &x: data["objects"].items() )
        {
            const std::string name = x.key();
            std::cout << name << " loading...";
            const json _json = x.value();
            Object *object = NULL;
            if( _json["type"] == "sphere" )
            {
                object = (Object *)new Sphere();
                //(Sphere *)object->radius = _json["radius"]; // <-- need to call different constructor?
            }
            else if( _json["type"] == "box" )
            {
                object = (Object *)new Box();
            }
            else if( _json["type"] == "plane" )
            {
                object = (Object *)new Plane();
            }
            else
            {
                std::cout << "unknown type: " << _json["type"] << std::endl;
                continue;
            }

            // Material props
            if( _json.contains("material") )
            {
                field_from_json(_json["material"], "reflective", 0.0, object->material.reflective);
                field_from_json(_json["material"], "specular", 0.0, object->material.specular);
                field_from_json(_json["material"], "usePattern", false, object->material.usePattern);
                std::string color_name;
                field_from_json(_json["material"], "color", 0.0, color_name);
                object->material.color = colors[color_name];

                // Pattern props
                if( _json["material"].contains("pattern"))
                {
                    // field_from_json(_mat, "pattern", "type", "unknown", object->material.pattern->type);

                    std::string pattern_color_name;
                    field_from_json(_json["material"]["pattern"], "color", 0.0, color_name);
                    // object->material.pattern->color = colors[pattern_color_name];

                    // object->material.pattern->color1 = Color(from_json["material"]["pattern"]["color1"]);
                    // object->material.pattern->color2 = Color(from_json["material"]["pattern"]["color2"]);

                    float _pattern_scale[3];
                    array_from_json(_json["material"]["pattern"], "scale", 1.0, _pattern_scale);
                    Position pattern_scale(_pattern_scale);

                    float _pattern_axis[3];
                    array_from_json(_json["material"]["pattern"], "axis", 0.0, _pattern_axis);
                    Direction pattern_axis(_pattern_axis);

                    float pattern_degrees;
                    field_from_json(_json["material"]["pattern"], "degrees", 0.0, pattern_degrees);

                    // Transform pattern
            		glm::mat4 scaling = ScaleMatrix(pattern_scale);

                    glm::mat4 rotation = glm::mat4(1.0);
                    if( pattern_degrees != 0.0 )
                    {
                        std::cout << "rotating pattern..." << std::endl;
                		rotation = RotateMatrix(pattern_axis,pattern_degrees);
                    }
            		glm::mat4 translate = TranslateMatrix(Position(0.0));

                    glm::mat4 xform = translate * rotation * scaling;

                    // object->material.pattern = new Pattern();
                    // object->material.pattern->SetTransform(translate * rotation * scaling);
                }

            }
            // Object props
            object->name = name;
            object->oid = _oid++;

            float _center[3];
            array_from_json(_json, "center", 0.0, _center);
            Position center(_center);

            float _scale[3];
            array_from_json(_json, "scale", 1.0, _scale);
            Position scale(_scale);

            float _axis[3];
            array_from_json(_json, "axis", 0.0, _axis);
            Direction axis(_axis);

            float degrees;
            field_from_json(_json, "degrees", 0.0, degrees);

            // Transform object
    		glm::mat4 scaling = ScaleMatrix(scale);
            glm::mat4 rotation = glm::mat4(1.0);
            if( degrees != 0.0 )
            {
                rotation = RotateMatrix(axis,degrees);
            }
    		glm::mat4 translate = TranslateMatrix(center);

            object->SetTransform(translate * rotation * scaling);

            std::cout << name << " loaded." << std::endl;
            objects.push_back(object);
        }
        std::cout<< "objects done." << std::endl;
        for( auto &x: data["lights"].items() )
        {
            const std::string name = x.key();
            auto _json = x.value();
            float center[3];
            float color[3];
            // array_from_json(_json, "center", 0.0, degrees);
            // array_from_json(_json["material"], "color", 0.0, color);

            Light *light = new Light(Position(center[0], center[1], center[2]));
            light->name = name;
            light->oid = _lid++;
            light->material.color = Color(color[0], color[1], color[2]);
            lights.push_back(light);
        }
        std::cout<< "lights done." << std::endl;
        object_count = objects.size();
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
				if( i.distance[0] < nearest.distance[0] )
				{
					nearest.gothit		= true;
					nearest.distance[0]	= i.distance[0];
					nearest.distance[1]	= i.distance[1];
					nearest.normal		= i.normal;
					nearest.oid			= i.oid;
					nearest.position	= i.position;
				}
				// For shadow rays, any hit will do (don't need the actual closest one)
				if( nearest.anyhit && nearest.gothit )
				{
					std::cout << nearest.distance[0] << " ";
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
