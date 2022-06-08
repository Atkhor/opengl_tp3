/*-----------------------------------------------------------------*/
/*!
  \file matrix4.hh
  \brief defines matrix class and utlity functions
*/
/*-----------------------------------------------------------------*/

#ifndef __MATRIX4__
#define __MATRIX4__

#include <GL/glew.h>

#include <GL/freeglut.h>
#include <iostream>
#include <vector>

class matrix4 {
public:
  matrix4();

  GLfloat vals[16];

  void operator*=(const matrix4 &rhs);

  void cross(matrix4 mat);
  static matrix4 identity();
  static matrix4 frustum(const GLfloat &left, const GLfloat &right,
                         const GLfloat &bottom, const GLfloat &top,
                         const GLfloat &z_near, const GLfloat &z_far);

  static matrix4 look_at(const GLfloat &eyeX, const GLfloat &eyeY,
                         const GLfloat &eyeZ, const GLfloat &centerX,
                         const GLfloat &centerY, const GLfloat &centerZ,
                         GLfloat upX, GLfloat upY, GLfloat upZ);
};

//! printer
inline std::ostream &operator<<(std::ostream &out, matrix4 mat) {

  out << "Matrix4 : " << std::endl;

  out << '[';
  for (int i = 0; i < 4; i++) {
    out << '[';
    for (int j = 0; j < 4; j++) {
      out << mat.vals[i * 4 + j];
      if (j != 3)
        out << ',';
    }
    out << ']';
    if (i != 3)
      out << std::endl;
  }
  out << ']' << std::endl;
  return out;
}

#endif