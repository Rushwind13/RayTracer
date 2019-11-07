#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;

struct TupleCtx
{
  glm::vec3 vec;
  glm::vec3 vec_b;
  Position pos;
  Position pos_b;
  glm::vec3 result;
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

WHEN("^I press divide_vector ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->vec / scalar;
}

WHEN("^I press divide_position ([0-9.-]+)$")
{
  REGEX_PARAM(float,scalar);
  ScenarioScope<TupleCtx> context;

  context->result = context->pos / scalar;
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;

  EXPECT_EQ(Position(x,y,z), context->result);
}

THEN("^the result should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a vector$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;

  EXPECT_EQ(glm::vec3(x,y,z), context->result);
}
