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
GIVEN("^I create a translation matrix ([MABCR])$")
{
  REGEX_PARAM(char, name);
  ScenarioScope<TestCtx> context;
  glm::mat4 input = TranslateMatrix(context->pos);
  switch(name)
  {
      case 'M':
      case 'A':
        context->mat = input;
        break;
      case 'B':
        context->mat_b = input;
        break;
      case 'C':
        context->mat_c = input;
        break;
      case 'R':
        context->result_mat = input;
        break;
  }
}

GIVEN("^I create a scaling matrix ([MABCR])$")
{
  REGEX_PARAM(char, name);
  ScenarioScope<TestCtx> context;
  glm::mat4 input = ScaleMatrix(context->pos);
  switch(name)
  {
      case 'M':
      case 'A':
        context->mat = input;
        break;
      case 'B':
        context->mat_b = input;
        break;
      case 'C':
        context->mat_c = input;
        break;
      case 'R':
        context->result_mat = input;
        break;
  }
}

GIVEN("^I create a rotation matrix ([MABCR]) ([0-9.-]+) degrees about ([XYZ])$")
{
  REGEX_PARAM(char, name);
  REGEX_PARAM(float, degrees);
  REGEX_PARAM(char, axis);
  ScenarioScope<TestCtx> context;
  glm::mat4 input = RotateMatrix(degrees, axis);
  switch(name)
  {
      case 'M':
      case 'A':
        context->mat = input;
        break;
      case 'B':
        context->mat_b = input;
        break;
      case 'C':
        context->mat_c = input;
        break;
      case 'R':
        context->result_mat = input;
        break;
  }
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
WHEN("^I transform the position ([0-9.-]+),([0-9.-]+),([0-9.-]+) with ([MABCR])$")
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
    case 'C':
      input = context->mat_c;
      break;
    case 'R':
      input = context->result_mat;
      break;
  }
  context->result_pos = input * context->pos;
}

WHEN("^I transform the vector ([0-9.-]+),([0-9.-]+),([0-9.-]+) with ([MABCR])$")
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
    case 'C':
      input = context->mat_c;
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
