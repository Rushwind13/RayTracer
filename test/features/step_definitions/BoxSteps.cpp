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
GIVEN("^I have a box$")
{
}

/*#######
##
## WHEN
##
#######*/
WHEN("^I intersect the ray with the box$")
{
    ScenarioScope<TestCtx> context;
    context->box.local_intersect(context->ray, context->intersection);
}



WHEN("^I calculate the box normal at ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  ScenarioScope<TestCtx> context;
  Position point_on_box(x,y,z);
  context->result_vec = context->box.NormalAt(point_on_box);
}
/*#######
##
## THEN
##
#######*/
