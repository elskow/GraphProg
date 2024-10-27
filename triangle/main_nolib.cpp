#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 ourColor;
void main() {
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

const char *fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

// Function pointers for OpenGL functions
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;

void loadOpenGLFunctions() {
  glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddress(
      (const GLubyte *)"glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddress(
      (const GLubyte *)"glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddress(
      (const GLubyte *)"glCompileShader");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress(
      (const GLubyte *)"glCreateProgram");
  glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddress(
      (const GLubyte *)"glAttachShader");
  glLinkProgram =
      (PFNGLLINKPROGRAMPROC)glXGetProcAddress((const GLubyte *)"glLinkProgram");
  glDeleteShader = (PFNGLDELETESHADERPROC)glXGetProcAddress(
      (const GLubyte *)"glDeleteShader");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glXGetProcAddress(
      (const GLubyte *)"glGenVertexArrays");
  glGenBuffers =
      (PFNGLGENBUFFERSPROC)glXGetProcAddress((const GLubyte *)"glGenBuffers");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glXGetProcAddress(
      (const GLubyte *)"glBindVertexArray");
  glBindBuffer =
      (PFNGLBINDBUFFERPROC)glXGetProcAddress((const GLubyte *)"glBindBuffer");
  glBufferData =
      (PFNGLBUFFERDATAPROC)glXGetProcAddress((const GLubyte *)"glBufferData");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glXGetProcAddress(
      (const GLubyte *)"glVertexAttribPointer");
  glEnableVertexAttribArray =
      (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress(
          (const GLubyte *)"glEnableVertexAttribArray");
  glUseProgram =
      (PFNGLUSEPROGRAMPROC)glXGetProcAddress((const GLubyte *)"glUseProgram");
  glGetShaderiv =
      (PFNGLGETSHADERIVPROC)glXGetProcAddress((const GLubyte *)"glGetShaderiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glXGetProcAddress(
      (const GLubyte *)"glGetShaderInfoLog");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glXGetProcAddress(
      (const GLubyte *)"glGetProgramiv");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glXGetProcAddress(
      (const GLubyte *)"glGetProgramInfoLog");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glXGetProcAddress(
      (const GLubyte *)"glDeleteBuffers");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glXGetProcAddress(
      (const GLubyte *)"glDeleteVertexArrays");

  // Check if any function pointers are null
  if (!glCreateShader || !glShaderSource || !glCompileShader ||
      !glCreateProgram || !glAttachShader || !glLinkProgram ||
      !glDeleteShader || !glGenVertexArrays || !glGenBuffers ||
      !glBindVertexArray || !glBindBuffer || !glBufferData ||
      !glVertexAttribPointer || !glEnableVertexAttribArray || !glUseProgram ||
      !glGetShaderiv || !glGetShaderInfoLog || !glGetProgramiv ||
      !glGetProgramInfoLog || !glDeleteBuffers || !glDeleteVertexArrays) {
    std::cerr << "Failed to load OpenGL functions" << std::endl;
    exit(-1);
  }
}

unsigned int compileShader(const char *source, GLenum type) {
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  return shader;
}

unsigned int createShaderProgram(const char *vertexSource,
                                 const char *fragmentSource) {
  unsigned int vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
  unsigned int fragmentShader =
      compileShader(fragmentSource, GL_FRAGMENT_SHADER);

  unsigned int program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, nullptr, infoLog);
    std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

void render(unsigned int shaderProgram) {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram);

  float vertices[] = {
      // positions         // colors
      0.0f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, // top (green)
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left (red)
      0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // bottom right (blue)
  };

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

int main() {
  Display *display = XOpenDisplay(NULL);
  if (!display) {
    std::cerr << "Cannot open display\n";
    return -1;
  }

  Window root = DefaultRootWindow(display);

  GLint attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
  XVisualInfo *vi = glXChooseVisual(display, 0, attribs);
  if (!vi) {
    std::cerr << "No appropriate visual found\n";
    return -1;
  }

  Colormap cmap = XCreateColormap(display, root, vi->visual, AllocNone);
  XSetWindowAttributes swa;
  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;

  Window win =
      XCreateWindow(display, root, 0, 0, WIDTH, HEIGHT, 0, vi->depth, InputOutput,
                    vi->visual, CWColormap | CWEventMask, &swa);
  XMapWindow(display, win);
  XStoreName(display, win, "No Lib");

  GLXContext glc = glXCreateContext(display, vi, NULL, GL_TRUE);
  glXMakeCurrent(display, win, glc);

  loadOpenGLFunctions();

  unsigned int shaderProgram =
      createShaderProgram(vertexShaderSource, fragmentShaderSource);

  XEvent xev;
  while (true) {
    XNextEvent(display, &xev);

    if (xev.type == Expose) {
      render(shaderProgram);
      glXSwapBuffers(display, win);
    }

    if (xev.type == KeyPress) {
      break;
    }
  }

  glXMakeCurrent(display, None, NULL);
  glXDestroyContext(display, glc);
  XDestroyWindow(display, win);
  XCloseDisplay(display);

  return 0;
}



