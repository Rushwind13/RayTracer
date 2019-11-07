#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;

struct TupleCtx
{
    Color col;
    Color col_b;
    glm::vec3 vec;
    glm::vec3 vec_b;
    Position pos;
    Position pos_b;
    glm::vec3 result;
    float result_b;
};

GIVEN("^I have a position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->pos = Position(x,y,z);
}

GIVEN("^I have a second position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->pos_b = Position(x,y,z);
}

GIVEN("^I have a color ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,r);
  REGEX_PARAM(double,g);
  REGEX_PARAM(double,b);
  ScenarioScope<TupleCtx> context;
  context->col = Color(r,g,b);
}

GIVEN("^I have a second color ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,r);
  REGEX_PARAM(double,g);
  REGEX_PARAM(double,b);
  ScenarioScope<TupleCtx> context;
  context->col_b = Color(r,g,b);
}

GIVEN("^I have a vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->vec = glm::vec3(x,y,z);
}

GIVEN("^I have a second vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->vec_b = glm::vec3(x,y,z);
}

WHEN("^I press add$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->pos + context->vec;
}

WHEN("^I press add_vector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->vec_b + context->vec;
}

WHEN("^I press add_color$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->col_b + context->col;
}

WHEN("^I press subtract$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->pos - context->vec;
}

WHEN("^I press subtract_point$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->pos - context->pos_b;
}

WHEN("^I press subtract_vector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->vec - context->vec_b;
}

WHEN("^I press subtract_color$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->col - context->col_b;
}

WHEN("^I press negate_vector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = -context->vec;
}

WHEN("^I press scale_vector ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->vec * scalar;
}

WHEN("^I press scale_position ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->pos * scalar;
}

WHEN("^I press scale_color ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->col * scalar;
}

WHEN("^I press divide_vector ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->vec / scalar;
}

WHEN("^I press multiply_color$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->col * context->col_b;
}

WHEN("^I press divide_position ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->pos / scalar;
}

WHEN("^I press magnitude$")
{
  ScenarioScope<TupleCtx> context;

  context->result_b = glm::length(context->vec);
}

WHEN("^I press normalize$")
{
  ScenarioScope<TupleCtx> context;

  context->result = glm::normalize(context->vec);
}

WHEN("^I press magnorm$")
{
  ScenarioScope<TupleCtx> context;

  context->result = glm::normalize(context->vec);
  context->result_b = glm::length(context->result);
}

WHEN("^I press dotprod$")
{
  ScenarioScope<TupleCtx> context;

  context->result_b = glm::dot(context->vec, context->vec_b);
}

WHEN("^I press dotprod_position$")
{
  ScenarioScope<TupleCtx> context;

  context->result_b = glm::dot(context->pos, context->pos_b);
}

WHEN("^I press crossprod$")
{
  ScenarioScope<TupleCtx> context;

  context->result = glm::cross(context->vec, context->vec_b);
}

WHEN("^I press ReflectVector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = ReflectVector(context->vec, context->vec_b);
}

WHEN("^I press Color_vector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = Color(context->vec);
}

WHEN("^I press Color$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->col;
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  Position expected(x,y,z);
  ScenarioScope<TupleCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a vector$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  glm::vec3 expected(x,y,z);
  ScenarioScope<TupleCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a color$")
{
  REGEX_PARAM(double,r);
  REGEX_PARAM(double,g);
  REGEX_PARAM(double,b);
  Color expected(r,g,b);
  ScenarioScope<TupleCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
}


THEN("^the result should be ([0-9.-]+) a float$")
{
  REGEX_PARAM(float,expected);
  ScenarioScope<TupleCtx> context;
  bool result = false;
  float EPSILON = 0.00001;
  if( glm::abs(expected - context->result_b) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, context->result_b);
}

THEN("^result.([xyzrgb]) should be ([0-9.-]+)$")
{
  REGEX_PARAM(char,axis);
  REGEX_PARAM(float,expected);
  ScenarioScope<TupleCtx> context;

  float compare = 1e9;
  switch(axis)
  {
    case 'x':
        compare = context->result.x;
        break;
    case 'y':
        compare = context->result.y;
        break;
    case 'z':
        compare = context->result.z;
        break;
    case 'r':
        compare = context->result.r;
        break;
    case 'g':
        compare = context->result.g;
        break;
    case 'b':
        compare = context->result.b;
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
