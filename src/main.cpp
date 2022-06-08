#include "matrix4.hh"
#include "program.hh"
#include <GL/glew.h>

#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <cmath>

const std::vector<GLfloat> vertex_{1.0, -0.5, 0.0,  0.0, 0.5,
                                   0.0, -1.0, -0.5, 0.0};

const std::vector<GLfloat> vertex{
    1,  1,  -1, -1, 1,  -1, -1, 1,  1,  1,  1,  -1, 1,  1,  1,  -1, 1,  1,
    1,  -1, -1, -1, -1, -1, -1, -1, 1,  1,  -1, -1, 1,  -1, 1,  -1, -1, 1,
    1,  1,  -1, 1,  -1, -1, -1, -1, -1, 1,  1,  -1, -1, 1,  -1, -1, -1, -1,
    1,  1,  1,  1,  -1, 1,  -1, -1, 1,  1,  1,  1,  -1, 1,  1,  -1, -1, 1,
    -1, 1,  -1, -1, -1, -1, -1, -1, 1,  -1, 1,  -1, -1, 1,  1,  -1, -1, 1,
    1,  1,  -1, 1,  -1, -1, 1,  -1, 1,  1,  1,  -1, 1,  1,  1,  1,  -1, 1};

program *prog = nullptr;
GLuint vao_id;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
}

void display() {
  // During init, enable debug ou
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  TEST_OPENGL_ERROR();
  glBindVertexArray(vao_id);
  TEST_OPENGL_ERROR();
  glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 3.0);
  TEST_OPENGL_ERROR();
  glBindVertexArray(0);
  TEST_OPENGL_ERROR();
  glutSwapBuffers();
}

double distance = 17;
double angle_alpha = 0;
double angle_beta = 0;
double sky_up = 1;

int old_pos_x = 0;
int old_pos_y = 0;

void update_position() {

  double p0=distance*cos(angle_alpha)*cos(angle_beta);
  double p1=distance*sin(angle_beta);
  double p2=distance*sin(angle_alpha)*cos(angle_beta);

 matrix4 model_view_matrix = matrix4::look_at(
	       p0, p1, p2,
         0, 0, 0,
  	     0, sky_up, 0
  );
  glUseProgram(prog->getProgID());
  GLint model_view_matrix_location =
    glGetUniformLocation(prog->getProgID(), "model_view_matrix");

  //std::cout << "model_view_matrix_location " << model_view_matrix_location << std::endl;
  glUniformMatrix4fv(model_view_matrix_location, 1, GL_TRUE, &model_view_matrix.vals[0]);


  matrix4 projection_matrix = matrix4::frustum( -0.05, 0.05,
      -0.05, 0.05,
      0.5, 50);
  GLint projection_matrix_location =
    glGetUniformLocation(prog->getProgID(), "projection_matrix");
  glUniformMatrix4fv(projection_matrix_location, 1, GL_TRUE, &projection_matrix.vals[0]);
  //std::cout << "projection_matrix_location " << projection_matrix_location << std::endl;
}

void mouse_motion_callback(int x, int y) {
  double alpha=angle_alpha-(old_pos_x-x)*M_PI/50.0;
  double beta =angle_beta-(old_pos_y-y)*M_PI/50.0;
  double sky;

  if (beta>M_PI/2.0) {
    beta=M_PI/2.0;
    sky=-1;
  } else if (beta<-M_PI/2.0) {
    beta=-M_PI/2.0;
    sky=-1;
  } else sky=1;

  if (alpha>M_PI) alpha-=2*M_PI;
  if (alpha<0) alpha+=2*M_PI;

  angle_alpha = alpha;
  angle_beta = beta;
  sky_up = sky;

  old_pos_x = x;
  old_pos_y = y;
  //  std::cout << "motion" << std::endl;
  update_position();
  glutPostRedisplay();
}

bool init_glut(int &argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1024, 1024);
  glutInitWindowPosition(10, 10);
  glutCreateWindow("Test OpenGL - POGL");
  glutDisplayFunc(display);
  glutMotionFunc(mouse_motion_callback);

  return true;
}

bool init_glew() { return (glewInit() == GLEW_OK); }

void init_gl() {
  glEnable(GL_DEPTH_TEST);
  TEST_OPENGL_ERROR();
  glDisable(GL_CULL_FACE);
  TEST_OPENGL_ERROR();
  glClearColor(0.2, 0.2, 0.2, 1.0);
  TEST_OPENGL_ERROR();
  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
}

std::string load(const std::string &filename) {
  std::ifstream input_src_file(filename, std::ios::in);
  std::string ligne;
  std::string file_content = "";
  if (input_src_file.fail()) {
    std::cerr << "FAIL\n";
    return "";
  }
  while (getline(input_src_file, ligne)) {
    file_content = file_content + ligne + "\n";
  }
  file_content += '\0';
  input_src_file.close();
  return file_content;
}

bool init_shaders() {
  std::string vertex_src = load("vertex.shd");
  std::string fragment_src = load("fragment.shd");

  prog = program::make_program(vertex_src, fragment_src);
  return prog->is_ready();
}

bool init_object() {
  prog->use();
  GLuint position_id = 0;
  GLint position_location = glGetAttribLocation(prog->getProgID(), "position");
  TEST_OPENGL_ERROR();
  glGenVertexArrays(1, &vao_id);
  TEST_OPENGL_ERROR();
  glBindVertexArray(vao_id);
  TEST_OPENGL_ERROR();
  glGenBuffers(1, &position_id);

  if (position_location >= 0) {
    glBindBuffer(GL_ARRAY_BUFFER, position_id);
    TEST_OPENGL_ERROR();
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(),
                 GL_STATIC_DRAW);
    TEST_OPENGL_ERROR();
    glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
    TEST_OPENGL_ERROR();
    glEnableVertexAttribArray(position_location);
    TEST_OPENGL_ERROR();
  } else {
    std::cerr << "init_object: position not found ! \n";
    return false;
  }

  // uniform color TODO: should be in display ?
  GLint color_location = glGetUniformLocation(prog->getProgID(), "color");
  // TODO: find how to link uniform with value
  if (color_location != -1) {
    glUniform3f(color_location, (GLfloat)0.1, (GLfloat)0.1, (GLfloat)0.8);
    TEST_OPENGL_ERROR();
  } else {
    std::cerr << "init_object: color_location not found ! \n";
    return false;
  }

  glBindVertexArray(0);

  return true;
}

bool init_POV() {

  // uniform model_view_mat
  GLint model_location =
      glGetUniformLocation(prog->getProgID(), "model_view_matrix");
  TEST_OPENGL_ERROR();
  if (model_location != -1) {
    matrix4 model_mat =
        matrix4::look_at(-0.1, 0.1, -0.05, 0, 0, 0, 0, 1.0, 0.0);
    // matrix4 model_mat = matrix4::identity();
    glUniformMatrix4fv(model_location, 1, GL_FALSE, &model_mat.vals[0]);
    TEST_OPENGL_ERROR();
  } else {
    std::cerr << "init_object: model_location not found ! \n";
    return false;
  }

  // uniform projection_mat
  GLint projection_location =
      glGetUniformLocation(prog->getProgID(), "projection_matrix");
  TEST_OPENGL_ERROR();
  if (projection_location != -1) {
    matrix4 projection_mat = matrix4::frustum(-3, 3, -3, 3, 0.1, 30.0);
    // matrix4 projection_mat = matrix4::identity();

    glUniformMatrix4fv(projection_location, 1, GL_FALSE,
                       &projection_mat.vals[0]);
    TEST_OPENGL_ERROR();
  } else {
    std::cerr << "init_object: projecion_location not found ! \n";
    return false;
  }

  return true;
}

int main(int argc, char **argv) {
  init_glut(argc, argv);

  std::cout << matrix4::look_at(5, 6, 7, 13, 14, 15, 0, 1, 0);
  // std::cout << matrix4::look_at(5, 6, 7, 13, 14, 15, 1, 0, 0);
  //  std::cout << matrix4::frustum(-5, 5, -5, 5, 1, 100);
  //  std::cout << matrix4::frustum(-3, 3, -6, 6, 2, 300);

  if (!init_glew()) {
    std::cerr << "error while initializing glew !\n";
    return 1;
  }

  init_gl();

  if (!init_shaders()) {
    return 1;
  }

  if (!init_object()) {
    return 1;
  }

  if (!init_POV()) {
    return 1;
  }

  glutMainLoop();

  return 0;
}
