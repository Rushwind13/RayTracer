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
GIVEN("^I have a sphere$")
{
}

/*#######
##
## WHEN
##
#######*/
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

WHEN("^I set the sphere's transform to ([MABCR])$")
{
  REGEX_PARAM(char, name);
  ScenarioScope<TestCtx> context;
  glm::mat4 input;
  switch(name)
  {
    case 'M':
    case 'A':
      input = context->mat;
      break;
    case 'B':
      input = context->mat_b;
      break;
    case 'C':
      input = context->mat_c;
      break;
    case 'R':
      input = context->result_mat;
      break;
  }
  context->sphere.SetTransform(input);
}

WHEN("^I set the sphere's material$")
{
  ScenarioScope<TestCtx> context;
  context->sphere.material = context->mtl;
}
/*#######
##
## THEN
##
#######*/
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


  bool result = false;

  if( glm::length((glm::vec4)actual) - 1.0 < epsilon &&
      glm::length(expected-actual) < epsilon )
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

THEN("^the sphere's material is the default$")
{
  ScenarioScope<TestCtx> context;
  Material expected = context->mtl;
  Material compare = context->sphere.material;

  bool result = false;

  if( glm::abs(expected.ambient - compare.ambient) < epsilon &&
      glm::abs(expected.diffuse - compare.diffuse) < epsilon &&
      glm::abs(expected.specular - compare.specular) < epsilon &&
      glm::abs(expected.shininess - compare.shininess) < epsilon &&
      glm::length(expected.color - compare.color) < epsilon )
  {
    result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, compare);
}
