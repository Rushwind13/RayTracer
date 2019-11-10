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

GIVEN("^I have a position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TestCtx> context;
  context->pos = Position(x,y,z);
}

GIVEN("^I have a second position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TestCtx> context;
  context->pos_b = Position(x,y,z);
}

GIVEN("^I have a position <([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+)> in the data$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  REGEX_PARAM(float, w);
  ScenarioScope<TestCtx> context;
  context->pos = glm::vec4(x,y,z,w);
}

GIVEN("^I have a color ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,r);
  REGEX_PARAM(float,g);
  REGEX_PARAM(float,b);
  ScenarioScope<TestCtx> context;
  context->col = Color(r,g,b);
}

GIVEN("^I have a second color ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,r);
  REGEX_PARAM(float,g);
  REGEX_PARAM(float,b);
  ScenarioScope<TestCtx> context;
  context->col_b = Color(r,g,b);
}

GIVEN("^I have a vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TestCtx> context;
  context->vec = Direction(x,y,z);
  // printvec("vec", context->vec);
}

GIVEN("^I have a second vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TestCtx> context;
  context->vec_b = Direction(x,y,z);
}


/*#######
##
## WHEN
##
#######*/

WHEN("^I press add$")
{
  ScenarioScope<TestCtx> context;

  context->result_float = context->pos.w;
  context->result_pos = context->pos + context->vec;
}

WHEN("^I press add_vector$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = context->vec_b + context->vec;
}

WHEN("^I press add_color$")
{
  ScenarioScope<TestCtx> context;

  context->result_col = context->col_b + context->col;
}

WHEN("^I press subtract$")
{
  ScenarioScope<TestCtx> context;

  context->result_float = context->pos.w;
  context->result_pos = context->pos - context->vec;
}

WHEN("^I press subtract_point$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = context->pos - context->pos_b;
}

WHEN("^I press subtract_vector$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = context->vec - context->vec_b;
}

WHEN("^I press subtract_color$")
{
  ScenarioScope<TestCtx> context;

  context->result_col = context->col - context->col_b;
}

WHEN("^I press negate_vector$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = -context->vec;
}

WHEN("^I press scale_vector ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TestCtx> context;

  context->result_vec = context->vec * scalar;
}

WHEN("^I press scale_position ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TestCtx> context;
  context->result_pos = (glm::vec4)context->pos * scalar;
}

WHEN("^I press scale_color ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TestCtx> context;

  context->result_col = context->col * scalar;
}

WHEN("^I press divide_vector ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TestCtx> context;

  context->result_vec = context->vec / scalar;
}

WHEN("^I press multiply_color$")
{
  ScenarioScope<TestCtx> context;

  context->result_col = context->col * context->col_b;
}

WHEN("^I press divide_position ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TestCtx> context;

  context->result_pos = context->pos / scalar;
  context->result_float = context->pos.w;
}

WHEN("^I press magnitude$")
{
  ScenarioScope<TestCtx> context;

  context->result_float = glm::length((glm::vec4)context->vec);
}

WHEN("^I press normalize$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = glm::normalize((glm::vec4)context->vec);
}

WHEN("^I press magnorm$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = glm::normalize((glm::vec4)context->vec);
  context->result_float = glm::length((glm::vec4)context->result_vec);
}

WHEN("^I press dotprod$")
{
  ScenarioScope<TestCtx> context;

  context->result_float = glm::dot((glm::vec4)context->vec, (glm::vec4)context->vec_b);
}

WHEN("^I press dotprod_position$")
{
  ScenarioScope<TestCtx> context;

  context->result_float = glm::dot((glm::vec4)context->pos, (glm::vec4)context->pos_b);
}

WHEN("^I press crossprod$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = glm::cross((glm::vec3)context->vec, (glm::vec3)context->vec_b);
}

WHEN("^I press ReflectVector$")
{
  ScenarioScope<TestCtx> context;

  context->result_vec = ReflectVector(context->vec, context->vec_b);
}

WHEN("^I press Color_vector$")
{
  ScenarioScope<TestCtx> context;

  context->result_col = Color(context->vec);
}

WHEN("^I press Color$")
{
  ScenarioScope<TestCtx> context;

  context->result_col = context->col;
}


/*#######
##
## THEN
##
#######*/

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  Position expected(x,y,z);
  ScenarioScope<TestCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_pos) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(context->result_pos.w, 1.0);
}


THEN("^Results in ([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  REGEX_PARAM(float,w);
  Position expected(x,y,z,w);
  ScenarioScope<TestCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_pos) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(context->result_pos.w, 1.0);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a vector$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  Direction expected(x,y,z);
  ScenarioScope<TestCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_vec) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(context->result.w, 0.0);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a color$")
{
  REGEX_PARAM(float,r);
  REGEX_PARAM(float,g);
  REGEX_PARAM(float,b);
  Color expected(r,g,b);
  ScenarioScope<TestCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_col) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(context->result_col.w, 0.0);
}


THEN("^the result should be ([0-9.-]+) a float$")
{
  REGEX_PARAM(float,expected);
  ScenarioScope<TestCtx> context;
  bool result = false;
  float EPSILON = 0.00001;
  if( glm::abs(expected - context->result_float) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, context->result_float);
}

THEN("^result.([xyzrgb]) should be ([0-9.-]+)$")
{
  REGEX_PARAM(char,axis);
  REGEX_PARAM(float,expected);
  ScenarioScope<TestCtx> context;

  float compare = 1e9;
  switch(axis)
  {
    case 'x':
        compare = context->result_vec.x;
        break;
    case 'y':
        compare = context->result_vec.y;
        break;
    case 'z':
        compare = context->result_vec.z;
        break;
    case 'r':
        compare = context->result_col.r;
        break;
    case 'g':
        compare = context->result_col.g;
        break;
    case 'b':
        compare = context->result_col.b;
        break;
    }
  bool result = false;
  float EPSILON = 0.00001;
  if( glm::abs(expected - compare) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, compare);
}

THEN("^Result is (a position|not a vector)$")
{
    ScenarioScope<TestCtx> context;
    EXPECT_NE(context->result_pos.w, 0.0);
}

THEN("^It is (a position|not a vector)$")
{
    ScenarioScope<TestCtx> context;
    EXPECT_NE(context->pos.w, 0.0);
}

THEN("^It is (a vector|a color|not a position)$")
{
    ScenarioScope<TestCtx> context;
    EXPECT_EQ(context->vec.w, 0.0);
}

THEN("^The scale did not change$")
{
    ScenarioScope<TestCtx> context;
    float expected = context->result_float;
    float compare = context->result_pos.w;

    bool result = false;
    float EPSILON = 0.00001;
    if( glm::abs(expected - compare) < EPSILON )
    {
      result = true;
    }

    EXPECT_EQ(result, true);
    // EXPECT_EQ(expected, compare);
}

THEN("^The scale changed to ([0-9.-]+)$")
{
    REGEX_PARAM(float, expected);
    ScenarioScope<TestCtx> context;
    float compare = context->result_pos.w;

    bool result = false;
    float EPSILON = 0.00001;
    if( glm::abs(expected - compare) < EPSILON )
    {
      result = true;
    }

    EXPECT_EQ(result, true);
    // EXPECT_EQ(expected, compare);
}
