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
GIVEN("^I have a material$")
{
  ScenarioScope<TestCtx> context;
  context->result_col = context->mtl.color;
}


/*#######
##
## WHEN
##
#######*/
WHEN("^I set m.ambient = ([0-9.-]+)$")
{
  REGEX_PARAM(float, ambient);
  ScenarioScope<TestCtx> context;
  context->mtl.ambient = ambient;
}

/*#######
##
## THEN
##
#######*/
THEN("^([MSO]) ambient = ([0-9.-]+)$")
{
  REGEX_PARAM(char, name);
  REGEX_PARAM(float, expected);
  ScenarioScope<TestCtx> context;
  float compare;

  switch(name)
  {
    case 'M':
      compare = context->mtl.ambient;
      break;
    case 'S':
      compare = context->sphere.material.ambient;
      break;
    case 'O':
      compare = context->shape.material.ambient;
      break;
  }

  bool result = false;

  if( glm::abs(expected - compare) < epsilon )
  {
    result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, compare);
}

THEN("^diffuse = ([0-9.-]+)$")
{
  REGEX_PARAM(float, expected);
  ScenarioScope<TestCtx> context;
  float compare = context->mtl.diffuse;
  bool result = false;

  if( glm::abs(expected - compare) < epsilon )
  {
    result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, compare);
}

THEN("^specular = ([0-9.-]+)$")
{
  REGEX_PARAM(float, expected);
  ScenarioScope<TestCtx> context;
  float compare = context->mtl.specular;
  bool result = false;

  if( glm::abs(expected - compare) < epsilon )
  {
    result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, compare);
}

THEN("^shininess = ([0-9.-]+)$")
{
  REGEX_PARAM(float, expected);
  ScenarioScope<TestCtx> context;
  float compare = context->mtl.shininess;
  bool result = false;

  if( glm::abs(expected - compare) < epsilon )
  {
    result = true;
  }

  EXPECT_EQ(result, true);
  // EXPECT_EQ(expected, compare);
}
