/* -*- coding: utf-8 -*- */

/** 
 * tuto-002-passVar.cpp
 *
 * Plus d'interaction avec les Shaders
 * http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_03
 */

//#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>      // std::ifstream 

// ******************************************************************** Window
/**
 *
 */
class Window
{
public:
  // ****************************************************************** CREATION
  /** Création avec titre et taille de fenetre.*/
  Window(const std::string& title = "GLFW Window", int width=640, int height=400) :
    _buffer(NULL)
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

    // // GLEW
    // GLenum glew_status = glewInit();
    // if (glew_status != GLEW_OK)
    //   {
    // 	std::cerr << "Error: " <<  glewGetErrorString(glew_status) << std::endl;
    // 	exit(EXIT_FAILURE);
    //   }
  }
  /** Destruction */
  ~Window() 
  {
    if( _buffer ) delete[] _buffer;
    glfwDestroyWindow(_window);

    // Détruit le programme GLSL
    glDeleteProgram(_program);
    // Et les vbo
    glDeleteBuffers(1, &_vbo_triangle);

    glfwTerminate();
    exit(EXIT_SUCCESS);
  }

  // ********************************************************************** INIT
  /** Charger des Shaders dans _buffer */
  void read_shader( const std::string& filename ) 
  {
    // Stream pour lire
    std::ifstream is ( filename.c_str(), std::ifstream::binary);
    if (is) {
      // get length of file:
      is.seekg (0, is.end);
      int length = is.tellg();
      is.seekg (0, is.beg);
      
      if(_buffer) delete[] _buffer;
      _buffer = new char[length];
      
      std::cout << filename << " reading " << length << " characters... ";
      // read data as a block:
      is.read (_buffer,length);
      
      if (is)
	std::cout << "all characters read successfully." << std::endl;
      else {
	std::cout << "error: only " << is.gcount() << " could be read" << std::endl;
	if(_buffer) delete[] _buffer;
	_buffer = NULL;
      }
      is.close();
    }
  }
  /** Donne des info sur la compilation de Shaders */
  std::string str_compiler_log(GLuint object) 
  {
    
    // Longueur du message
    GLint log_length = 0;
    if (glIsShader(object))
      glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
      glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else {
      return std::string("'object' n'est ni un Shader ni un Program GLSL");
    }

    // Copie le message dans string
    char* log = new char[log_length+1];
    if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);
    log[log_length] = '\0';

    std::string str_log(log);
    delete[] log;

    return str_log;
  }
  /**
   * Compile un Shader d'après un fichier. Avec erreur de compilation.
   *
   * @param type : GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER
   */
  GLuint create_shader(const std::string& filename, GLenum type)
  {
    read_shader(filename);
    if (_buffer == NULL) {
      std::cerr << "Erreur avec " << filename << std::endl;
      return 0;
    }
    
    // Crée le Shader
    GLuint res = glCreateShader(type);
    const GLchar* sources[2] = {
#ifdef GL_ES_VERSION_2_0
      "#version 100\n"
      "#define GLES2\n",
#else
      "#version 120\n",
#endif
      _buffer };
    // Lie avec les sources lues
    glShaderSource(res, 2, sources, NULL);
    // Copile
    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    // Si pb, on affiche les erreurs.
    if (compile_ok == GL_FALSE) {
      std::cerr << "__" << filename << " : Pb de COMPILATION" << std::endl;
      std::cerr << str_compiler_log(res) << std::endl;
      glDeleteShader(res);
      if( _buffer ) delete[] _buffer;
      _buffer = NULL;
      return 0;
    }

    if( _buffer ) delete[] _buffer;
    _buffer = NULL;
    return res;
  }
  
  int init_resources(void)
  {
#ifdef GL_ES_VERSION_2_0
    std::cout << "OpenGL ES 2.0" << std::endl;
#else
    std::cout << "OpenGL ES 2.1" << std::endl;
#endif
    GLint link_ok = GL_FALSE;

    GLuint vs, fs;
    /** Vertex Shader */
    // Transforme les coordonnées du modèle en coordonnées écran.
    if ((vs = create_shader("../opengltuto/triangle-003.v.glsl", GL_VERTEX_SHADER))   == 0)
      return 0;
    /** Fragment Shader */
    // Que faire avec tous les points dans le triangle
    // Couleur dépend des coordonnées
    // Peindre en bleu
    if ((fs = create_shader("../opengltuto/triangle-003.f.glsl", GL_FRAGMENT_SHADER)) == 0)
      return 0;

    /** GLSL program */
    // link Vertex et Fragment Shaders
    // program est une variable GLOBALE
    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      std::cerr << "__Pb avec glLinkProgam "<< std::endl;
      std::cerr << str_compiler_log(_program) << std::endl;
      return 0;
    }

    /** Passer les données */
    // La variable GLOBALE attribute_coord2d est créée, en lien avec
    // la variable 'coord2d' du programme GLSL
    const char* attribute_name = "coord2d";
    _attribute_coord2d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord2d == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      return 0;
    }
    // la variable 'v_color' du programme GLSL
    attribute_name = "v_color";
    _attribute_v_color = glGetAttribLocation(_program, attribute_name);
    if (_attribute_v_color == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      return 0;
    }

    // Définition du triangle dans un VBO (Vertex Buffer Object)
    // Les points du triangle
    GLfloat triangle_vertices[] = {
      0.0,  0.8,
      -0.8, -0.8,
      0.8, -0.8,
    };
    // Un VBO
    glGenBuffers(1, &_vbo_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_triangle);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), 
		 triangle_vertices, GL_STATIC_DRAW);
    // Qu'on peut rendre inactif avec
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Les couleurs du triangle
    GLfloat triangle_colors[] = {
    1.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    };
    glGenBuffers(1, &_vbo_triangle_colors);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_triangle_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors),
		 triangle_colors, GL_STATIC_DRAW);
    
    return 1;
  }
  
  // ******************************************************************** RENDER
  void display_cbk()
  {
    /* Clear the background as white */
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
 
    glUseProgram(_program);
    
    // Triangle 
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_triangle );
    glEnableVertexAttribArray(_attribute_coord2d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord2d, // attribute
      2,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // no extra data between each position
      0                  // offset of first element
			  );
    // Color
    glEnableVertexAttribArray(_attribute_v_color);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_triangle_colors);
    glVertexAttribPointer(
      _attribute_v_color, // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // no extra data between each position
      0                  // offset of first element
			  );
 
    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(_attribute_coord2d);
    glDisableVertexAttribArray(_attribute_v_color);
  }
  
  void render()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
  GLint _attribute_coord2d, _attribute_v_color;;
  /** Vertex Buffer Object avec des triangles */
  GLuint _vbo_triangle, _vbo_triangle_colors;
  /** Buffer pour lire des shaders */
  char* _buffer;
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
  if( win.init_resources() == 1 )
    win.render();
  return 0;
}
