#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;
#include "TestContext.hpp"

GIVEN("^I have a sphere$")
{
}

WHEN("^I intersect the ray with the sphere$")
{
    ScenarioScope<TestCtx> context;
    context->sphere.Intersect(context->ray, context->intersection);
}

WHEN("^I calculate the normal at ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  ScenarioScope<TestCtx> context;
  Position point_on_sphere(x,y,z);
  context->result_vec = context->sphere.NormalAt(point_on_sphere);
}

THEN("^intersection count = ([0-9]+)$")
{
    // REGEX_PARAM(int, count);
    // ScenarioScope<TestCtx> context;
    // EXPECT_EQ(context->intersection.count, count);
}

THEN("^an intersection occurred$")
{
    ScenarioScope<TestCtx> context;
    EXPECT_EQ(context->intersection.gothit, true);
}

THEN("^no intersection occurred$")
{
    ScenarioScope<TestCtx> context;
    EXPECT_EQ(context->intersection.gothit, false);
}

THEN("^intersection distance ([0-9]+) = ([0-9.-]+)$")
{
    REGEX_PARAM(int, index);
    REGEX_PARAM(float, distance);
    ScenarioScope<TestCtx> context;
    EXPECT_EQ(context->intersection.distance, distance);
}

THEN("^the vector is normalized$")
{
  ScenarioScope<TestCtx> context;
  Direction actual = context->result_vec;
  Direction expected = glm::normalize(context->result_vec);

  float EPSILON = 0.00001;
  bool result = false;

  if( glm::length((glm::vec4)actual) - 1.0 < EPSILON &&
      glm::length(expected-actual) < EPSILON )
  {
      result = true;
  }
  else
  {
      printvec("e", expected);
      std::cout << std::endl;
      printvec("a", actual);
      std::cout << std::endl;
      result = false;
  }

  EXPECT_EQ(result, true);
}
