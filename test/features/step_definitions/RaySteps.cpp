#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;


struct RayCtx
{
  Position origin;
  glm::vec3 direction;
  Ray ray;
  Ray result;
};

GIVEN("^I have a position ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<RayCtx> context;
  context->origin = Position(x,y,z);
}

GIVEN("^I have a vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) in the data$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<RayCtx> context;
  context->direction = glm::vec3(x,y,z);
}

WHEN("^I press Ray$")
{
  ScenarioScope<RayCtx> context;

  context->result = Ray(context->origin, context->direction);
}



THEN("^the origin should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  Position expected(x,y,z);
  ScenarioScope<RayCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result.origin) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
}

THEN("^the direction should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a vector$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  glm::vec3 expected(x,y,z);
  ScenarioScope<RayCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result.direction) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
}


THEN("^the length should be ([0-9.-]+) a float$")
{
  REGEX_PARAM(float,expected);
  ScenarioScope<RayCtx> context;
  bool result = false;
  float EPSILON = 0.00001;
  if( glm::abs(expected - glm::length(context->result.direction)) < EPSILON )
  {
      result = true;
  }

  // EXPECT_EQ(result, true);
  EXPECT_EQ(expected, glm::length(context->result.direction));
}