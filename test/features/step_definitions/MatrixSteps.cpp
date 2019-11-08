#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <Math.hpp>
using cucumber::ScenarioScope;

struct MatrixCtx
{
  glm::mat4 mat;
  glm::mat3 mat3;
  glm::mat2 mat2;
  glm::mat4 mat_b;
  glm::vec4 pos;
  glm::mat4 id = glm::mat4(1);
};

GIVEN("^the following matrix ([MAB]):$")
{
  REGEX_PARAM(char, name);
  TABLE_PARAM(table);
  ScenarioScope<MatrixCtx> context;
  glm::mat4 input;
  const table_hashes_type & rows = table.hashes();
  int col = 0;
  for (table_hashes_type::const_iterator it = rows.begin(); it != rows.end(); ++it)
  {
      float x = ::cucumber::internal::fromString<float>(it->at("x"));
      float y = ::cucumber::internal::fromString<float>(it->at("y"));
      float z = ::cucumber::internal::fromString<float>(it->at("z"));
      float w = ::cucumber::internal::fromString<float>(it->at("w"));

      input[col++] = glm::vec4(x,y,z,w);
  }
  switch(name)
  {
    case 'M':
    case 'A':
      context->mat = glm::transpose(input);
      break;
    case 'B':
      context->mat_b = glm::transpose(input);
      break;
  }
}

GIVEN("^the following 3x3 matrix M:$")
{
  TABLE_PARAM(table);
  ScenarioScope<MatrixCtx> context;

  const table_hashes_type & rows = table.hashes();
  int col = 0;
  for (table_hashes_type::const_iterator it = rows.begin(); it != rows.end(); ++it)
  {
      float x = ::cucumber::internal::fromString<float>(it->at("x"));
      float y = ::cucumber::internal::fromString<float>(it->at("y"));
      float z = ::cucumber::internal::fromString<float>(it->at("z"));
      context->mat3[col++] = glm::vec3(x,y,z);
  }
  context->mat3 = glm::transpose(context->mat3);
}

GIVEN("^the following 2x2 matrix M:$")
{
  TABLE_PARAM(table);
  ScenarioScope<MatrixCtx> context;

  const table_hashes_type & rows = table.hashes();
  int col = 0;
  for (table_hashes_type::const_iterator it = rows.begin(); it != rows.end(); ++it)
  {
      float x = ::cucumber::internal::fromString<float>(it->at("x"));
      float y = ::cucumber::internal::fromString<float>(it->at("y"));
      context->mat2[col++] = glm::vec2(x,y);
  }
  context->mat2 = glm::transpose(context->mat2);
}

GIVEN("^the following position <([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+)>$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  REGEX_PARAM(float, w);
  ScenarioScope<MatrixCtx> context;
  context->pos = glm::vec4(x,y,z,w);
}

THEN("^M([234])<([0-9]),([0-9])> = ([0-9.-]+)$")
{
  REGEX_PARAM(char, dim);
  REGEX_PARAM(int, row);
  REGEX_PARAM(int, col);
  REGEX_PARAM(float, expected);
  ScenarioScope<MatrixCtx> context;
  float result = 1e9;
  switch(dim)
  {
    case '2':
      result = context->mat2[col][row];
      break;
    case '3':
      result = context->mat3[col][row];
      break;
    case '4':
      result = context->mat[col][row];
      break;
  }

  EXPECT_EQ(result, expected);
}

THEN("^A = B$")
{
  ScenarioScope<MatrixCtx> context;
  EXPECT_EQ(context->mat, context->mat_b);
}

THEN("^A != B$")
{
  ScenarioScope<MatrixCtx> context;
  EXPECT_NE(context->mat, context->mat_b);
}


THEN("^A \\* B is the following 4x4 matrix:$")
{
  TABLE_PARAM(table);
  ScenarioScope<MatrixCtx> context;
  glm::mat4 expected;
  const table_hashes_type & rows = table.hashes();
  int col = 0;
  for (table_hashes_type::const_iterator it = rows.begin(); it != rows.end(); ++it)
  {
      float x = ::cucumber::internal::fromString<float>(it->at("x"));
      float y = ::cucumber::internal::fromString<float>(it->at("y"));
      float z = ::cucumber::internal::fromString<float>(it->at("z"));
      float w = ::cucumber::internal::fromString<float>(it->at("w"));

      expected[col++] = glm::vec4(x,y,z,w);
  }
  expected = glm::transpose(expected);

  glm::mat4 result = context->mat * context->mat_b;
  // std::cout << result[0][0] << ' ' << result[1][1] << ' ' << result[2][2] << ' ' << result[3][3] << std::endl;

  EXPECT_EQ(result, expected);
}

THEN("^A \\* b = <([0-9.-]+),([0-9.-]+),([0-9.-]+),([0-9.-]+)> is a position$")
{
  REGEX_PARAM(float, x);
  REGEX_PARAM(float, y);
  REGEX_PARAM(float, z);
  REGEX_PARAM(float, w);
  ScenarioScope<MatrixCtx> context;
  glm::vec4 expected(x,y,z,w);
  glm::vec4 result = context->mat * context->pos;

  EXPECT_EQ(result, expected);
}

THEN("^A \\* I = A$")
{
    ScenarioScope<MatrixCtx> context;
    glm::mat4 result = context->mat * context->id;
    EXPECT_EQ(result, context->mat);
}
