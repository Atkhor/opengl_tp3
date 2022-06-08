/*-----------------------------------------------------------------*/
/*!
  \file matrix4.cpp
  \brief implements matrix class and utlity functions
*/
/*-----------------------------------------------------------------*/

#include "matrix4.hh"
#include <math.h>

matrix4::matrix4() {
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++)
      vals[i * 4 + j] = 0;
  }
}

void matrix4::operator*=(const matrix4 &rhs) {
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++)
      vals[i * 4 + j] *= rhs.vals[i * 4 + j];
  }
}

matrix4 matrix4::frustum(const GLfloat &left, const GLfloat &right,
                         const GLfloat &bottom, const GLfloat &top,
                         const GLfloat &nearVal, const GLfloat &farVal) {
  matrix4 res = matrix4();

  GLfloat A = (right + left) / (right - left);
  GLfloat B = (top + bottom) / (top - bottom);
  GLfloat C = -(farVal + nearVal) / (farVal - nearVal);
  GLfloat D = -(2 * farVal * nearVal) / (farVal - nearVal);

  res.vals[0] = 2 * nearVal / (right - left);
  res.vals[2] = A;

  res.vals[5] = 2 * nearVal / (top - bottom);
  res.vals[6] = B;

  res.vals[10] = C;
  res.vals[11] = D;

  res.vals[14] = -1;

  return res;
}

matrix4 matrix4::look_at(const GLfloat &eyeX, const GLfloat &eyeY,
                         const GLfloat &eyeZ, const GLfloat &centerX,
                         const GLfloat &centerY, const GLfloat &centerZ,
                         GLfloat upX, GLfloat upY, GLfloat upZ) {
  matrix4 res = matrix4();

  GLfloat fx = centerX - eyeX;
  GLfloat fy = centerY - eyeY;
  GLfloat fz = centerZ - eyeZ;

  GLfloat sx, sy, sz;
  GLfloat sn;
  GLfloat ux, uy, uz;

  GLfloat nf = std::sqrt(fx * fx + fy * fy + fz * fz);

  GLfloat nu = std::sqrt(upX * upX + upY * upY + upZ * upZ);

  fx /= nf;
  fy /= nf;
  fz /= nf;

  upX /= nu;
  upY /= nu;
  upZ /= nu;

  sx = fy * upZ - fz * upY;
  sy = fz * upX - fx * upZ;
  sz = fx * upY - fy * upX;


  ux = sy * fz - sz * fy;
  uy = sz * fx - sx * fz;
  uz = sx * fy - sy * fx;

  res.vals[0] = sx;
  res.vals[1] = sy;
  res.vals[2] = sz;

  res.vals[4] = ux;
  res.vals[5] = uy;
  res.vals[6] = uz;

  res.vals[8] = -fx;
  res.vals[9] = -fy;
  res.vals[10] = -fz;

  res.vals[15] = 1;

  matrix4 temp = matrix4::identity();
  temp.vals[3] = -eyeX;
  temp.vals[7] = -eyeY;
  temp.vals[11] = -eyeZ;

  res.cross(temp);

  return res;
}

matrix4 matrix4::identity() {
  matrix4 res = matrix4();
  res.vals[0] = 1;
  res.vals[1 * 4 + 1] = 1;
  res.vals[2 * 4 + 2] = 1;
  res.vals[3 * 4 + 3] = 1;
  return res;
}

void matrix4::cross(matrix4 mat) {
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      double temp = 0;
      for (size_t k = 0; k < 4; k++)
        temp += vals[i * 4 + k] * mat.vals[k * 4 + j];
      vals[i * 4 + j] = temp;
    }
  }
}