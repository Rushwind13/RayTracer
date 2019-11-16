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

  printmat("input", input);
  context->shape.SetTransform(input);
  printmat("o2w",context->shape.objectToWorld);
}

WHEN("^I set the shape's material$")
{
  ScenarioScope<TestCtx> context;
  context->shape.material = context->mtl;
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
