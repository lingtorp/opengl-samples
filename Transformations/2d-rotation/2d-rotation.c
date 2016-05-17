#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdint.h>
#include <stdlib.h> // calloc, free
#include <stdbool.h>
#include <math.h>

// Assumes the file exists and will seg. fault otherwise.
const GLchar *load_shader_source(char *filename) {
  FILE *file = fopen(filename, "r");             // open
  fseek(file, 0L, SEEK_END);                     // find the end
  size_t size = ftell(file);                     // get the size in bytes
  GLchar *shaderSource = calloc(1, size);        // allocate enough bytes
  rewind(file);                                  // go back to file beginning
  fread(shaderSource, size, sizeof(char), file); // read each char into ourblock
  fclose(file);                                  // close the stream
  return shaderSource;
}

GLfloat *transformation_matrix_y(float theta) {
  float x = M_PI / 180;
  GLfloat *matrix = calloc(1, sizeof(float) * 9);
  GLfloat tempMatrix[3][3] = {{1.0f, 0.0f, 0.0f},
                              {0.0f, cos(theta * x), -sin(theta * x)},
                              {0.0f, sin(theta * x), cos(theta * x)}};
  memccpy(matrix, tempMatrix, '\n', sizeof(float) * 9);
  return matrix;
}

GLfloat *transformation_matrix_z(float theta) {
  float x = M_PI / 180;
  GLfloat *matrix = calloc(1, sizeof(float) * 9);
  GLfloat tempMatrix[3][3] = {{cos(theta * x), 0.0f, sin(theta * x)},
                              {0.0f, 1.0f, 0.0f},
                              {-sin(theta * x), 0.0f, cos(theta * x)}};
  memccpy(matrix, tempMatrix, '\n', sizeof(float) * 9);
  return matrix;
}

GLfloat *transformation_matrix_x(float theta) {
  float x = M_PI / 180;
  GLfloat *matrix = calloc(1, sizeof(float) * 9);
  GLfloat tempMatrix[3][3] = {{cos(theta * x), -sin(theta * x), 0.0f},
                              {sin(theta * x), cos(theta * x), 0.0f},
                              {0.0f, 0.0f, 1.0f}};
  memccpy(matrix, tempMatrix, '\n', sizeof(float) * 9);
  return matrix;
}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_Window *window =
      SDL_CreateWindow("OpenGL with SDL2", 0, 0, 500, 500,
                       SDL_WINDOW_OPENGL | SDL_WINDOWPOS_CENTERED);

  SDL_GLContext *context = SDL_GL_CreateContext(window);

  glewExperimental = true;
  glewInit();

  GLfloat vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                        0.0f,  0.0f,  0.5f, 0.0f};

  GLuint VBO;            // VBO - vertex buffer object
  glGenBuffers(1, &VBO); // generate a 'name for the object'

  glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind/make VBO the active object

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices,
               GL_STATIC_DRAW); // upload VBO to OpenGL

  const GLchar *vertexShaderSource = load_shader_source("vertex-shader.glsl");
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  GLint vertexShaderStatus;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderStatus);

  const GLchar *fragmentShaderSource =
      load_shader_source("fragment-shader.glsl");
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  GLint fragmentShaderStatus;
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderStatus);

  printf("Fragment: %i, Vertex: %i\n", fragmentShaderStatus,
         vertexShaderStatus);
  char buffer[512];
  glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
  printf("%s\n", buffer);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Bind Vertex Array Object - OpenGL core REQUIRES a VERTEX ARRAY OBJECT in
  // order to render ANYTHING.
  // An object that stores vertex array bindings
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO); // make the VAO the active one ..

  // Then set our vertex attributes pointers, only doable AFTER linking
  GLuint positionAttrib = glGetAttribLocation(shaderProgram, "position");
  glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(positionAttrib);

  GLuint trans = glGetUniformLocation(shaderProgram, "trans");
  printf("Trans Mat.: %u\n", trans);
  GLfloat *testMat =
      transformation_matrix_z(90.0f); // z <== x, x <== y, y <== z
  for (size_t i = 0; i < 9; i++) {
    printf("%f ", testMat[i]);
    if (i == 2 || i == 5 || i == 8) {
      printf("\n");
    }
  }

  bool DONE = false;
  float theta = 0.0f;
  GLfloat *transMat;
  while (!DONE) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        DONE = true;
      }
    }
    theta = theta + 2.0f;
    // Draw the object
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    transMat = transformation_matrix_z(theta);
    glUniformMatrix3fv(trans, 1, GL_TRUE, transMat);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    SDL_GL_SwapWindow(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
