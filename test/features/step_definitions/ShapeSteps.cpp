#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

using cucumber::ScenarioScope;
#include "TestContext.hpp"

/*#######
##
## GIVEN
##
#######*/
GIVEN("^I have a shape$")
{
}


/*#######
##
## WHEN
##
#######*/
WHEN("^I set the shape's transform to ([MABCR])$")
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
  context->shape.SetTransform(input);
}

WHEN("^I set the shape's material$")
{
  ScenarioScope<TestCtx> context;
  context->shape.material = context->mtl;
}

WHEN("^I intersect the ray with the shape$")
{
    ScenarioScope<TestCtx> context;
    context->shape.Intersect(context->ray, context->intersection);
    context->result_ray = context->shape.object_ray;
}

WHEN("^I calculate the shape normal at ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  ScenarioScope<TestCtx> context;
  Position point_on_shape(x,y,z);
  context->result_vec = context->shape.NormalAt(point_on_shape);
}

/*#######
##
## THEN
##
#######*/
THEN("^the shape's material is the default$")
{
  ScenarioScope<TestCtx> context;
  Material expected = context->mtl;
  Material compare = context->shape.material;

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

THEN("^intersection object = ([P])$")
{
  REGEX_PARAM(char,name);
  ScenarioScope<TestCtx> context;
  EXPECT_EQ(context->intersection.oid, context->plane.oid);
}

THEN("^intersection distance ([0-9]+) = ([0-9.-]+)$")
{
    REGEX_PARAM(int, index);
    REGEX_PARAM(float, distance);
    ScenarioScope<TestCtx> context;
    EXPECT_EQ(context->intersection.distance[index], distance);
}
