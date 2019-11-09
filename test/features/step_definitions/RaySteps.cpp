#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;


struct RayCtx
{
  Position origin;
  Direction direction;
  Ray ray;
  Ray result;
};

GIVEN("^the following position ([0-9.-]+),([0-9.-]+),([0-9.-]+) = origin$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<RayCtx> context;
  context->origin = Position(x,y,z);
}

GIVEN("^the following vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) = direction$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  ScenarioScope<RayCtx> context;
  context->direction = Direction(x,y,z);
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
  Direction expected(x,y,z);
  ScenarioScope<RayCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length((glm::vec4)expected - (glm::vec4)context->result.direction) < EPSILON )
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
  if( glm::abs(expected - glm::length((glm::vec4)context->result.direction)) < EPSILON )
  {
      result = true;
  }

  // EXPECT_EQ(result, true);
  EXPECT_EQ(expected, glm::length((glm::vec4)context->result.direction));
}
