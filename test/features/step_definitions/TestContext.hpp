#ifndef __TEST_CONTEXT__
#define __TEST_CONTEXT__

/*#######
##
## CONTEXT
##
#######*/
struct TestCtx
{
  glm::mat4 mat;
  glm::mat4 mat_b;
  glm::mat4 id = glm::mat4(1);

  glm::mat3 mat3;
  glm::mat2 mat2;

  Position pos;
  Position pos_b;

  Direction vec;
  Direction vec_b;

  Color col;
  Color col_b;

  Position origin;
  Direction direction;
  Ray ray;

  glm::mat4 result_mat;
  Position result_pos;
  Direction result_vec;
  Color result_col;
  float result_float;
  Ray result_ray;
};

#endif // __TEST_CONTEXT__
