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
  // printvec("v->", context->pos);
  // printvec("<-m", context->mat[3]);
}

/*#######
##
## WHEN
##
#######*/
WHEN("^I transform the position ([0-9.-]+),([0-9.-]+),([0-9.-]+)$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  ScenarioScope<TestCtx> context;
  context->pos = Position(x,y,z);
  context->result_float = context->pos.w;
  context->result_pos = context->mat * context->pos;
  // std::cout << std::endl;
  // printmat("xf", context->mat);
  // printvec("xp", context->pos);
  // std::cout << std::endl;
  // printvec("rp", context->result_pos);
  // std::cout << std::endl;
}

/*#######
##
## THEN
##
#######*/
