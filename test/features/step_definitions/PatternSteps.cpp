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
WHEN("^I set the shape's pattern to stripe$")
{
    ScenarioScope<TestCtx> context;
    context->shape.material.usePattern = true;
    context->shape.material.pattern = &(context->stripe);
}

WHEN("^I set the stripe's transform to ([MABCR])$")
{
  REGEX_PARAM(char, name);
  ScenarioScope<TestCtx> context;
  glm::mat4 input;
  switch(name)
  {
    case 'M':
    case 'A':
      input = context->mat;
      break;
    case 'B':
      input = context->mat_b;
      break;
    case 'C':
      input = context->mat_c;
      break;
    case 'R':
      input = context->result_mat;
      break;
  }
  context->stripe.SetTransform(input);
}

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
    Position pattern_pos(x,y,z);
    Color compare = context->stripe.PatternAt(pattern_pos);
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
    Position pattern_pos(x,y,z);
    Color compare = context->stripe.PatternAt(pattern_pos);
    Color expected = COLOR_BLACK;
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

THEN("^stripe_at_shape ([0-9.-]+),([0-9.-]+),([0-9.-]+) is white$")
{
    REGEX_PARAM(float, x);
    REGEX_PARAM(float, y);
    REGEX_PARAM(float, z);
    ScenarioScope<TestCtx> context;
    Position world_pos(x,y,z);
    Color compare = context->shape.ColorAt(world_pos);
    Color expected = COLOR_WHITE;
    bool result = false;
    if( glm::length(expected - compare) < epsilon )
    {
        result = true;
    }
    EXPECT_EQ(result, true);
}

THEN("^pointer cleanup occurred$")
{
    ScenarioScope<TestCtx> context;
    context->shape.material.usePattern = false;
    context->shape.material.pattern = NULL;
}