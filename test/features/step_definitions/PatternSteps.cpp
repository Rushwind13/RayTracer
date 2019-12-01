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
GIVEN("^Black is the color 0\\.1,0\\.1,0\\.1$")
{
    Color compare = COLOR_BLACK;
    Color expected(0.1,0.1,0.1);
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

GIVEN("^White is the color 1\\.0,1\\.0,1\\.0$")
{
    Color compare = COLOR_WHITE;
    Color expected(1.0,1.0,1.0);
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

GIVEN("^I have a stripe pattern$")
{
    ScenarioScope<TestCtx> context;
}
/*#######
##
## WHEN
##
#######*/


/*#######
##
## THEN
##
#######*/
THEN("^pattern.a = white$")
{
    ScenarioScope<TestCtx> context;
    Color compare = context->stripe.a;
    Color expected = COLOR_WHITE;
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

THEN("^pattern.b = black$")
{
    ScenarioScope<TestCtx> context;
    Color compare = context->stripe.b;
    Color expected = COLOR_BLACK;
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

THEN("^stripe_at ([0-9.-]+),([0-9.-]+),([0-9.-]+) is white$")
{
    REGEX_PARAM(float, x);
    REGEX_PARAM(float, y);
    REGEX_PARAM(float, z);
    ScenarioScope<TestCtx> context;
    Position check_pos(x,y,z);
    Color compare = context->stripe.PatternAt(check_pos);
    Color expected = COLOR_WHITE;
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

THEN("^stripe_at ([0-9.-]+),([0-9.-]+),([0-9.-]+) is black$")
{
    REGEX_PARAM(float, x);
    REGEX_PARAM(float, y);
    REGEX_PARAM(float, z);
    ScenarioScope<TestCtx> context;
    Position check_pos(x,y,z);
    Color compare = context->stripe.PatternAt(check_pos);
    Color expected = COLOR_BLACK;
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}
