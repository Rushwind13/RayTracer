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
