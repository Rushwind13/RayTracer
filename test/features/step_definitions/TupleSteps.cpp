#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;

struct TupleCtx
{
    Color col;
    Color col_b;
    Direction vec;
    Direction vec_b;
    Position pos;
    Position pos_b;
    Direction result;
    Position result_pos;
    Color result_col;
    float result_b;
};

GIVEN("^I have a position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TupleCtx> context;
  context->pos = Position(x,y,z);
  // printvec("pos", context->pos);
}

GIVEN("^I have a second position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TupleCtx> context;
  context->pos_b = Position(x,y,z);
}

GIVEN("^I have a color ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,r);
  REGEX_PARAM(float,g);
  REGEX_PARAM(float,b);
  ScenarioScope<TupleCtx> context;
  context->col = Color(r,g,b);
}

GIVEN("^I have a second color ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,r);
  REGEX_PARAM(float,g);
  REGEX_PARAM(float,b);
  ScenarioScope<TupleCtx> context;
  context->col_b = Color(r,g,b);
}

GIVEN("^I have a vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TupleCtx> context;
  context->vec = Direction(x,y,z);
  // printvec("vec", context->vec);
}

GIVEN("^I have a second vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  ScenarioScope<TupleCtx> context;
  context->vec_b = Direction(x,y,z);
}

WHEN("^I press add$")
{
  ScenarioScope<TupleCtx> context;

  context->result_pos = context->pos + context->vec;
  // printvec("result", context->result_pos);
  // std::cout << std::endl;
}

WHEN("^I press add_vector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->vec_b + context->vec;
}

WHEN("^I press add_color$")
{
  ScenarioScope<TupleCtx> context;

  context->result_col = context->col_b + context->col;
}

WHEN("^I press subtract$")
{
  ScenarioScope<TupleCtx> context;

  context->result_pos = context->pos - context->vec;
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

  context->result_col = context->col - context->col_b;
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

  context->result_pos = context->pos * scalar;
}

WHEN("^I press scale_color ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result_col = context->col * scalar;
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

  context->result_col = context->col * context->col_b;
}

WHEN("^I press divide_position ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result_pos = context->pos / scalar;
}

WHEN("^I press magnitude$")
{
  ScenarioScope<TupleCtx> context;

  context->result_b = glm::length((glm::vec4)context->vec);
}

WHEN("^I press normalize$")
{
  ScenarioScope<TupleCtx> context;

  context->result = glm::normalize((glm::vec4)context->vec);
}

WHEN("^I press magnorm$")
{
  ScenarioScope<TupleCtx> context;

  context->result = glm::normalize((glm::vec4)context->vec);
  context->result_b = glm::length((glm::vec4)context->result);
}

WHEN("^I press dotprod$")
{
  ScenarioScope<TupleCtx> context;

  context->result_b = glm::dot((glm::vec4)context->vec, (glm::vec4)context->vec_b);
}

WHEN("^I press dotprod_position$")
{
  ScenarioScope<TupleCtx> context;

  context->result_b = glm::dot((glm::vec4)context->pos, (glm::vec4)context->pos_b);
  std::cout << "dot(pos,pos_b) = " << context->result_b << std::endl;
}

WHEN("^I press crossprod$")
{
  ScenarioScope<TupleCtx> context;

  context->result = glm::cross((glm::vec3)context->vec, (glm::vec3)context->vec_b);
}

WHEN("^I press ReflectVector$")
{
  ScenarioScope<TupleCtx> context;

  context->result = ReflectVector(context->vec, context->vec_b);
}

WHEN("^I press Color_vector$")
{
  ScenarioScope<TupleCtx> context;

  context->result_col = Color(context->vec);
}

WHEN("^I press Color$")
{
  ScenarioScope<TupleCtx> context;

  context->result_col = context->col;
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  Position expected(x,y,z);
  ScenarioScope<TupleCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_pos) < EPSILON )
  {
      result = true;
  }
  // printvec("->", context->result_pos);
  // std::cout << std::endl;

  EXPECT_EQ(result, true);
  EXPECT_EQ(context->result_pos.w, 1.0);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a vector$")
{
  REGEX_PARAM(float,x);
  REGEX_PARAM(float,y);
  REGEX_PARAM(float,z);
  Direction expected(x,y,z);
  ScenarioScope<TupleCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  EXPECT_EQ(context->result.w, 0.0);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a color$")
{
  REGEX_PARAM(float,r);
  REGEX_PARAM(float,g);
  REGEX_PARAM(float,b);
  Color expected(r,g,b);
  ScenarioScope<TupleCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_col) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
  EXPECT_EQ(context->result_col.w, 0.0);
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

THEN("^It is (a position|not a vector)$")
{
    ScenarioScope<TupleCtx> context;
    EXPECT_EQ(context->pos.w, 1.0);
}

THEN("^It is (a vector|a color|not a position)$")
{
    ScenarioScope<TupleCtx> context;
    EXPECT_EQ(context->vec.w, 0.0);
}
