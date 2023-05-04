/*
 *  Loader.hpp
 *
 *  Created on: May 4, 2023
 *  Author: jimbo
 */

#ifndef LOADER_H_
#define LOADER_H_
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;



#define field_from_json( data, item, default_value, target) \
{ \
    if( data.contains(item) ) \
    { \
        target = data[item]; \
        std::cout << item << " " << target << std::endl; \
    } \
    else \
    { \
        target = default_value; \
    } \
}

#define array_from_json( data, item, default_value, target) \
{ \
    if( data.contains(item) ) \
    { \
        std::cout << item << " " << data[item] << std::endl; \
        target[0] = data[item][0]; \
        target[1] = data[item][1]; \
        target[2] = data[item][2]; \
    } \
    else \
    { \
        target[0] = default_value; \
        target[1] = default_value; \
        target[2] = default_value; \
    } \
}

#endif /* LOADER_H_ */
