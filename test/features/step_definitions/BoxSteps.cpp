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

/*#######
##
## THEN
##
#######*/
