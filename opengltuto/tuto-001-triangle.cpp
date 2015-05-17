/* -*- coding: utf-8 -*- */

/** 
 * tuto-001-triangle.cpp
 *
 * Essai pour faire un triable avec OpenGL 2.1
 * http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Introduction
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdlib.h>
#include <iostream>
 
// ******************************************************************** Window
/**
 *
 */
class Window
{
public:
  // ****************************************************************** CREATION
  /** Création avec titre et taille de fenetre.*/
  Window(const std::string& title = "GLFW Window", int width=640, int height=400)
  {
    std::cout << "Window creation" << std::endl;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!_window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(_window);
    glfwSetKeyCallback(_window, key_callback);

    // GLEW
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK)
      {
	std::cerr << "Error: " <<  glewGetErrorString(glew_status) << std::endl;
	exit(EXIT_FAILURE);
      }
  }
  /** Destruction */
  ~Window() 
  {
    glfwDestroyWindow(_window);

    // Détruit le programme GLSL
    glDeleteProgram(_program);

    glfwTerminate();
    exit(EXIT_SUCCESS);
  }

  // ********************************************************************** INIT
  int init_resources(void)
  {
#ifdef GL_ES_VERSION_2_0
    std::cout << "OpenGL ES 2.0" << std::endl;
#else
    std::cout << "OpenGL ES 2.1" << std::endl;
#endif
    GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

    /** Vertex Shader */
    // Transforme les coordonnées du modèle en coordonnées écran.
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vs_source = 
#ifdef GL_ES_VERSION_2_0
      "#version 100\n"  // OpenGL ES 2.0
#else
      "#version 120\n"  // OpenGL 2.1
#endif
      "attribute vec2 coord2d;                  "
      "void main(void) {                        "
      "  gl_Position = vec4(coord2d, 0.0, 1.0); "
      "}";
    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
    if (0 == compile_ok) {
      fprintf(stderr, "Error in vertex shader\n");
      return 0;
    }

    /** Fragmen Shader */
    // Que faire avec tous les points dans le triangle
    // Couleur dépend des coordonnées
    // Peindre en bleu
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fs_source =
      "#version 120           \n"
      "void main(void) {        "
      "  gl_FragColor[0] = gl_FragCoord.x/640.0;"
      "  gl_FragColor[1] = gl_FragCoord.y/480.0;"
      "  gl_FragColor[2] = 0.5; "
      "}";
    //  "  gl_FragColor[0] = 0.0; "
    //  "  gl_FragColor[1] = 0.0; "
    //  "  gl_FragColor[2] = 1.0; "

    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok) {
      fprintf(stderr, "Error in fragment shader\n");
      return 0;
    }

    /** GLSL program */
    // link Vertex et Fragment Shaders
    // program est une variable GLOBALE
    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      fprintf(stderr, "glLinkProgram:");
      return 0;
    }

    /** Passer les données */
    // La variable GLOBALE attribute_coord2d est créée, en lien avec
    // la variable 'coord2d' du programme GLSL
    const char* attribute_name = "coord2d";
    _attribute_coord2d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord2d == -1) {
      fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
      return 0;
    }
    
    return 1;
  }
  
  // ******************************************************************** RENDER
  void display_cbk()
  {
    /* Clear the background as white */
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
 
    glUseProgram(_program);
    glEnableVertexAttribArray(_attribute_coord2d);
    // Les points du triangle
    GLfloat triangle_vertices[] = {
      0.0,  0.8,
      -0.8, -0.8,
      0.8, -0.8,
    };
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord2d, // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // no extra data between each position
      triangle_vertices  // pointer to the C array
			  );
 
    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(_attribute_coord2d);
  }
  
  void render()
  {
    while (!glfwWindowShouldClose(_window)) {
      float ratio;
      int width, height;
      
      glfwGetFramebufferSize(_window, &width, &height);
      ratio = width / (float) height;
      
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      
      display_cbk();

      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  }
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord2d;
  //******************************************************************************
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }
  // ***************************************************************************
  /**
   * Callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
    //fputs(description, stderr);
  }
};

//******************************************************************************
int main( int argc, char *argv[] )
{
  Window win("Un triangle Moderne", 600, 600);
  win.init_resources();
  win.render();
  return 0;
}
