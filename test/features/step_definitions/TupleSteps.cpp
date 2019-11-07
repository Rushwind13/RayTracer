#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;

struct TupleCtx
{
  glm::vec3 vec;
  glm::vec3 pos;
  glm::vec3 result;
};

GIVEN("^I have a position 1,0,0 in the data$")
{
  // REGEX_PARAM(double,x);
  // REGEX_PARAM(double,y);
  // REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->pos = glm::vec3(1,0,0);
}

GIVEN("^I have a vector 0,1,0 in the data$")
{
  // REGEX_PARAM(double,x);
  // REGEX_PARAM(double,y);
  // REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;
  context->vec = glm::vec3(0,1,0);
}

WHEN("^I press add$")
{
  ScenarioScope<TupleCtx> context;

  context->result = context->pos + context->vec;
}

THEN("^the result should be 1,1,0 on the screen$")
{
  // REGEX_PARAM(double,x);
  // REGEX_PARAM(double,y);
  // REGEX_PARAM(double,z);
  ScenarioScope<TupleCtx> context;

  EXPECT_EQ(glm::vec3(1,1,0), context->result);
}
