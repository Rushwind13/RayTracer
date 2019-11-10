#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;
#include "TestContext.hpp"

GIVEN("^I have a Ray with origin ([0-9.-]+),([0-9.-]+),([0-9.-]+) and direction ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
    REGEX_PARAM(float, ox);
    REGEX_PARAM(float, oy);
    REGEX_PARAM(float, oz);
    REGEX_PARAM(float, dx);
    REGEX_PARAM(float, dy);
    REGEX_PARAM(float, dz);

    ScenarioScope<TestCtx> context;
    context->pos = Position(ox,oy,oz);
    context->vec = Direction(dx,dy,dz);

    context->ray = Ray(context->pos, context->vec);
    // printvec("ro->", context->ray.origin);
    // printvec("rd->", context->ray.direction);
}

WHEN("^I press Ray$")
{
  ScenarioScope<TestCtx> context;

  context->result_ray = Ray(context->pos, context->vec);
}


THEN("^the origin should be ([0-9.-]+),([0-9.-]+),([0-9.-]+) a position$")
{
  REGEX_PARAM(double,x);
  REGEX_PARAM(double,y);
  REGEX_PARAM(double,z);
  Position expected(x,y,z);
  ScenarioScope<TestCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length(expected - context->result_ray.origin) < EPSILON )
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
  ScenarioScope<TestCtx> context;

  bool result = false;
  float EPSILON = 0.00001;
  if( glm::length((glm::vec4)expected - (glm::vec4)context->result_ray.direction) < EPSILON )
  {
      result = true;
  }

  EXPECT_EQ(result, true);
}


THEN("^the length should be ([0-9.-]+) a float$")
{
  REGEX_PARAM(float,expected);
  ScenarioScope<TestCtx> context;
  bool result = false;
  float EPSILON = 0.00001;
  if( glm::abs(expected - context->result_ray.length) < EPSILON )
  {
      result = true;
  }

  // EXPECT_EQ(result, true);
  EXPECT_EQ(expected, context->result_ray.length);
  EXPECT_EQ(glm::length((glm::vec4)context->result_ray.direction), 1.0);
}

THEN("^position r,([0-9.-]+) = ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
    REGEX_PARAM(float, t);
    REGEX_PARAM(float, x);
    REGEX_PARAM(float, y);
    REGEX_PARAM(float, z);
    ScenarioScope<TestCtx> context;
    Position expected(x,y,z);

    context->result_pos = context->ray.origin + (context->ray.direction * t);
}
