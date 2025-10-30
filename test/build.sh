#!/bin/sh -x
# Suppress verbose linker warnings from libc++ visibility mismatches
g++ -o bin/AllSteps features/step_definitions/AllSteps.cpp \
-L/usr/local/lib -Wl,-w -lcucumber-cpp -lc++ \
-lboost_program_options -lboost_regex -lboost_filesystem -lboost_system -lgtest \
-I../libRaytracer2014/src -I/usr/local/include \
-std=c++11 -Wno-comment -Wno-delete-non-virtual-dtor -Wno-deprecated-declarations
#gcc -o bin/MatrixSteps features/step_definitions/MatrixSteps.cpp -L/usr/local/lib -lcucumber-cpp -lc++ -lboost_program_options -lboost_regex -lboost_filesystem -lboost_system -lgtest -I../libRaytracer2014/src -std=c++11 -Wno-comment -Wdelete-abstract-non-virtual-dtor
#gcc -o bin/TupleSteps features/step_definitions/TupleSteps.cpp -L/usr/local/lib -lcucumber-cpp -lc++ -lboost_program_options -lboost_regex -lboost_filesystem -lboost_system -lgtest -I../libRaytracer2014/src -std=c++11 -Wno-comment -Wdelete-abstract-non-virtual-dtor
#gcc -o bin/RaySteps features/step_definitions/RaySteps.cpp -L/usr/local/lib -lcucumber-cpp -lc++ -lboost_program_options -lboost_regex -lboost_filesystem -lboost_system -lgtest -I../libRaytracer2014/src -std=c++11 -Wno-comment -Wdelete-abstract-non-virtual-dtor
