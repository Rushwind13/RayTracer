#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;

struct TupleCtx
{
  glm::vec3 vec;
  glm::vec3 vec_b;
  Position pos;
  glm::vec3 result;
};

GIVEN("^I have a position ([0-9.]+),([0-9.]+),([0-9.]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->pos = Position(x,y,z);
}

GIVEN("^I have a vector ([0-9.]+),([0-9.]+),([0-9.]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->vec = glm::vec3(x,y,z);
}

GIVEN("^I have a second vector ([0-9.]+),([0-9.]+),([0-9.]+) in the data$")
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

THEN("^the result should be ([0-9.]+),([0-9.]+),([0-9.]+) a position$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;

  EXPECT_EQ(Position(x,y,z), context->result);
}

THEN("^the result should be ([0-9.]+),([0-9.]+),([0-9.]+) a vector$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;

  EXPECT_EQ(glm::vec3(x,y,z), context->result);
}
