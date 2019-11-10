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
GIVEN("^I create a translation matrix$")
{
  ScenarioScope<TestCtx> context;
  context->mat = TranslateMatrix(context->pos);
}

GIVEN("^I create a scaling matrix$")
{
  ScenarioScope<TestCtx> context;
  context->mat = ScaleMatrix(context->pos);
}

GIVEN("^I create a rotation matrix ([0-9.-]+) degrees about ([XYZ])$")
{
  REGEX_PARAM(float, degrees);
  REGEX_PARAM(char, axis);
  ScenarioScope<TestCtx> context;
  context->mat = RotateMatrix(degrees, axis);
}

GIVEN("^I create a shearing matrix <([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+)>$")
{
    REGEX_PARAM(float, xy);
    REGEX_PARAM(float, xz);
    REGEX_PARAM(float, yx);
    REGEX_PARAM(float, yz);
    REGEX_PARAM(float, zx);
    REGEX_PARAM(float, zy);
    ScenarioScope<TestCtx> context;
    context->mat = ShearMatrix(xy,xz,yx,yz,zx,zy);
}

/*#######
##
## WHEN
##
#######*/
WHEN("^I transform the position ([0-9.-]+),([0-9.-]+),([0-9.-]+) with ([MABR])$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  REGEX_PARAM(char, name);
  ScenarioScope<TestCtx> context;
  context->pos = Position(x,y,z);
  context->result_float = context->pos.w;
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
    case 'R':
      input = context->result_mat;
      break;
  }
  context->result_pos = input * context->pos;
}

WHEN("^I transform the vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) with ([MABR])$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  REGEX_PARAM(char, name);
  ScenarioScope<TestCtx> context;
  context->vec = Direction(x,y,z);
  context->result_float = context->vec.w;
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
    case 'R':
      input = context->result_mat;
      break;
  }
  context->result_vec = input * context->vec;
}

/*#######
##
## THEN
##
#######*/
