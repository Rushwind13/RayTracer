#!/bin/sh -x
gcc -o features/step_definitions/TupleSteps features/step_definitions/TupleSteps.cpp -L/usr/local/lib -lcucumber-cpp -lc++ -lboost_program_options -lboost_regex -lboost_filesystem -lboost_system -lgtest -I../libRaytracer2014/src -std=c++11
