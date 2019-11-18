#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;
#include "TestContext.hpp"

/*#######
##
## GIVEN
##
#######*/
GIVEN("^I have a sphere$")
{
}

/*#######
##
## WHEN
##
#######*/
WHEN("^I intersect the ray with the sphere$")
{
    ScenarioScope<TestCtx> context;
    context->sphere.local_intersect(context->ray, context->intersection);
}

WHEN("^I calculate the normal at ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  ScenarioScope<TestCtx> context;
  Position point_on_sphere(x,y,z);
  context->result_vec = context->sphere.local_normal_at(point_on_sphere);
}

/*#######
##
## THEN
##
#######*/
