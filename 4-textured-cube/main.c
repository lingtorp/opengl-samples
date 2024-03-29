#include "main.h"

Cube new_cube(Color4 *colors) {
  Cube cube;
  Point3 a = new_point(-0.5f, -0.5f, 0.5f);
  Point3 b = new_point(0.5f, -0.5f, 0.5f);
  Point3 c = new_point(0.5f, 0.5f, 0.5f);
  Point3 d = new_point(-0.5f, 0.5f, 0.5f);
  Point3 vertices1[] = {a, b, c, d};
  Quad quad1 = new_quad(vertices1, colors);
  cube.quads[0] = quad1;

  Point3 e = new_point(-0.5f, -0.5f, -0.5f);
  Point3 f = new_point(0.5f, -0.5f, -0.5f);
  Point3 g = new_point(0.5f, 0.5f, -0.5f);
  Point3 h = new_point(-0.5f, 0.5f, -0.5f);
  Point3 vertices2[] = {e, f, g, h};
  Quad quad2 = new_quad(vertices2, colors);
  cube.quads[1] = quad2;
  return cube;
}

GLfloat *cube_to_floats(Cube *cube) {
  GLfloat *cube_floats = calloc(1, sizeof(Quad) * 6);
  for (size_t i = 0; i < 6; i++) {
    Quad quad = cube->quads[i];
    GLfloat *quad_floats = quad_to_floats(&quad);
    int k = 0;
    for (size_t j = i * sizeof(Quad); j < sizeof(Quad); j++) {
      cube_floats[j] = quad_floats[k];
      k++;
    }
  }
  return cube_floats;
}

Vertex new_vertex(Point3 point, Color4 color) {
  Vertex vertice;
  vertice.color = color;
  vertice.position = point;
  return vertice;
}

// Input: 4 points, 4 colors
//  d ----- c   <-- Point & color pairs
//  |       |
//  a ----- b   0 = a, 1 = b, 2 = c, 3 = d
Quad new_quad(Point3 *points, Color4 *colors) {
  Quad quad;
  Point3 a = points[0];
  Point3 b = points[1];
  Point3 c = points[2];
  Point3 d = points[3];
  quad.vertices[0] = new_vertex(a, colors[0]);
  quad.vertices[1] = new_vertex(b, colors[1]);
  quad.vertices[2] = new_vertex(c, colors[2]);
  quad.vertices[3] = new_vertex(d, colors[3]);
  return quad;
}

// Converts a quad to vertices
// 1 quad = 2 triangles => 4 vertices = 4 points & 4 colors
GLfloat *quad_to_floats(Quad *quad) {
  GLfloat *floats = calloc(1, 4 * sizeof(Vertex));
  int j = 0;
  for (size_t i = 0; i < 4 * 7; i += 7) {
    Vertex vertex = quad->vertices[j];
    floats[i] = vertex.position.x;
    floats[i + 1] = vertex.position.y;
    floats[i + 2] = vertex.position.z;
    floats[i + 3] = vertex.color.r;
    floats[i + 4] = vertex.color.g;
    floats[i + 5] = vertex.color.b;
    floats[i + 6] = vertex.color.a;
    j++;
    printf("x: %f y: %f z: %f r: %f g: %f b: %f a: %f\n", vertex.position.x,
           vertex.position.y, vertex.position.z, vertex.color.a, vertex.color.g,
           vertex.color.b, vertex.color.a);
  }
  return floats;
}

Point3 new_point(GLfloat x, GLfloat y, GLfloat z) {
  Point3 point;
  point.x = x;
  point.y = y;
  point.z = z;
  return point;
}

Color4 new_color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
  Color4 color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;
  return color;
}

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

// Transformation matrix for the y-plane
GLfloat *transformation_matrix_x(float theta) {
  float x = M_PI / 180;
  GLfloat *matrix = calloc(1, sizeof(GLfloat) * 4 * 4);
  GLfloat tempMatrix[4][4] = {{1.0f, 0.0f, 0.0f, 0.0f},
                              {0.0f, cos(theta * x), -sin(theta * x), 0.0f},
                              {0.0f, sin(theta * x), cos(theta * x), 0.0f},
                              {0.0f, 0.0f, 0.0f, 1.0f}};
  memccpy(matrix, tempMatrix, '\n', sizeof(tempMatrix));
  return matrix;
}

GLfloat *transformation_matrix_y(float theta) {
  float x = M_PI / 180;
  GLfloat *matrix = calloc(1, sizeof(GLfloat) * 4 * 4);
  GLfloat tempMatrix[4][4] = {{cos(theta * x), 0.0f, sin(theta * x), 0.0f},
                              {0.0f, 1.0f, 0.0f, 0.0f},
                              {-sin(theta * x), 0.0f, cos(theta * x), 0.0f},
                              {0.0f, 0.0f, 0.0f, 1.0f}};
  memccpy(matrix, tempMatrix, '\n', sizeof(tempMatrix));
  return matrix;
}

GLfloat *transformation_matrix_z(float theta) {
  float x = M_PI / 180;
  GLfloat *matrix = calloc(1, sizeof(GLfloat) * 4 * 4);
  GLfloat tempMatrix[4][4] = {{cos(theta * x), -sin(theta * x), 0.0f, 0.0f},
                              {sin(theta * x), cos(theta * x), 0.0f, 0.0f},
                              {0.0f, 0.0f, 1.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f, 1.0f}};
  memccpy(matrix, tempMatrix, '\n', sizeof(tempMatrix));
  return matrix;
}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_Window *window = SDL_CreateWindow("", 0, 0, 500, 500, SDL_WINDOW_OPENGL);
  SDL_GLContext *context = SDL_GL_CreateContext(window);

  glewExperimental = true;
  glewInit();

  Color4 colors[] = {new_color_red(), new_color_green(), new_color_blue(),
                     new_color_yellow()};

  Cube cube = new_cube(colors);

  GLuint VBO;                         // VBO - vertex buffer object
  glGenBuffers(1, &VBO);              // generate a 'name for the object'
  glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind/make VBO the active object
  glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), NULL,
               GL_STATIC_DRAW);    // reserve a large buffer for cube quads
  for (size_t i = 0; i < 2; i++) { // upload all the cubes quads
    Quad quad = cube.quads[i];
    GLfloat *quad_data = quad_to_floats(&quad);
    glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(Quad), sizeof(Quad), quad_data);
  }

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

  printf("Position offset: %lu Color offset: %lu\n", offsetof(Vertex, position),
         offsetof(Vertex, color));
  // Then set our vertex attributes pointers, only doable AFTER linking
  GLuint positionAttrib = glGetAttribLocation(shaderProgram, "position");
  glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void *)offsetof(Vertex, position));
  glEnableVertexAttribArray(positionAttrib);

  GLuint colorAttrib = glGetAttribLocation(shaderProgram, "vColor");
  glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const void *)offsetof(Vertex, color));
  glEnableVertexAttribArray(colorAttrib);

  GLuint transform_x = glGetUniformLocation(shaderProgram, "transform_x");
  GLuint transform_y = glGetUniformLocation(shaderProgram, "transform_y");
  GLuint transform_z = glGetUniformLocation(shaderProgram, "transform_z");
  printf("Trans Mats.: %u %u %u\n", transform_x, transform_y, transform_z);
  // z <== x, x <== y, y <== z
  GLfloat *testMat = transformation_matrix_x(90.0f);
  for (size_t i = 0; i < 9; i++) {
    printf("%f ", testMat[i]);
    if (i == 2 || i == 5 || i == 8) {
      printf("\n");
    }
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                  GL_REPEAT); // tex.coords (s, t, r) == (x, y, z)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  IMG_Init(IMG_INIT_PNG); // init sdl2_image
  SDL_Surface *image = IMG_Load("retro.png");
  int width = image->w;
  int height = image->h;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image->pixels);
  // https://wiki.libsdl.org/SDL_PixelFormatEnum#type
  printf("W: %i  H: %i Format: %u\n", width, height,
         SDL_PIXELTYPE(image->format->format));

  GLuint texAttrib = glGetAttribLocation(shaderProgram, "vTexcoord");
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(texAttrib);

  GLuint indices[] = {// front
                      0, 1, 2, 2, 3, 0,
                      // right
                      1, 5, 6, 6, 2, 1,
                      // back
                      7, 6, 5, 5, 4, 7,
                      // left
                      4, 0, 3, 3, 7, 4,
                      // bot
                      4, 5, 1, 1, 0, 4,
                      // top
                      3, 2, 6, 6, 7, 3};

  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  bool DONE = false;
  float theta_x = 0.0f;
  float theta_y = 0.0f;
  float theta_z = 0.0f;
  GLfloat *transMat_x;
  GLfloat *transMat_y;
  GLfloat *transMat_z;
  while (!DONE) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_LEFT:
          theta_z += 6.0f;
          break;
        case SDLK_RIGHT:
          theta_z -= 6.0f;
          break;
        case SDLK_UP:
          theta_x -= 6.0f;
          break;
        case SDLK_DOWN:
          theta_x += 6.0f;
          break;
        }
        break;
      case SDL_QUIT:
        DONE = true;
        break;
      }
    }
    // Draw the object
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK);    // cull back face
    glFrontFace(GL_CCW);    // GL_CCW for counter clock-wise
    transMat_x = transformation_matrix_x(theta_x);
    glUniformMatrix4fv(transform_x, 1, GL_TRUE, transMat_x);
    transMat_y = transformation_matrix_y(theta_y);
    glUniformMatrix4fv(transform_y, 1, GL_TRUE, transMat_y);
    transMat_z = transformation_matrix_z(theta_z);
    glUniformMatrix4fv(transform_z, 1, GL_TRUE, transMat_z);
    // GOTTA BIND THE VAO TO TELL OPENGL WHERE THE INDICES ARE FROM
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
    SDL_GL_SwapWindow(window);
  }

  IMG_Quit();
  free(transMat_x);
  free(transMat_y);
  free(transMat_z);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
