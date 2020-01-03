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
GIVEN("^I have a plane$")
{

}


/*#######
##
## WHEN
##
#######*/
WHEN("^I calculate the plane normal at ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  ScenarioScope<TestCtx> context;
  Position point_on_plane(x,y,z);
  context->result_vec = context->plane.local_normal_at(point_on_plane);
}

WHEN("^I intersect the ray with the plane$")
{
    ScenarioScope<TestCtx> context;
    context->plane.local_intersect(context->ray, context->intersection);
}

/*#######
##
## THEN
##
#######*/
