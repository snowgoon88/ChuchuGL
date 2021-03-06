/* -*- coding: utf-8 -*- */

/** 
 * tuto-004-transform.cpp
 *
 * Matrices de projection et autres
 * http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_04
 * Se souvenir
 * Transformations are applied by multiplying 4x4 matrices in the reverse order. M = M_translation * M_rotation means rotate first, then translate.
 * The identity matrix is the matrix that does nothing - no transformation at all.
 * To transform a vertex, we multiply it by the matrix: v' = M * v
 * 4x4 matrices can be applied to 4x1 vectors only, which we obtain by using 1 in the 4th dimensions for vertices: (x, y, z, 1).
 */

//#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GLFW/glfw3.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>      // std::ifstream 
#include <chrono>         //std::chrono

// Une structure pour les données
struct attributes {
  GLfloat coord3d[3];
  GLfloat v_color[3];
};

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
      _buffer = new char[length+1];
      
      std::cout << filename << " reading " << length << " characters... ";
      // read data as a block:
      is.read (_buffer,length);
      _buffer[length] = '\0';
      if (is) {
 	std::cout << "all characters read successfully." << std::endl;
	std::cout << "-----" << std::endl;
	std::cout << _buffer << std::endl;
	std::cout << "-----" << std::endl;
      }
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
    if ((vs = create_shader("../opengltuto/triangle-004.v.glsl", GL_VERTEX_SHADER))   == 0)
      return 0;
    /** Fragment Shader */
    // Que faire avec tous les points dans le triangle
    // Couleur dépend des coordonnées
    // Peindre en bleu
    if ((fs = create_shader("../opengltuto/triangle-004.f.glsl", GL_FRAGMENT_SHADER)) == 0)
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
    const char* attribute_name = "coord3d";
    _attribute_coord3d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord3d == -1) {
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
    // la variable uniform 'fade' du programme GLSL
    const char* uniform_name = "fade";
    _uniform_fade = glGetUniformLocation(_program, uniform_name);
    if (_uniform_fade == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      return 0;
    }
    // la variable uniforme m_transform
    uniform_name = "m_transform";
    _uniform_m_transform = glGetUniformLocation(_program, uniform_name);
    if (_uniform_m_transform == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      return 0;
    }

    // Définition du triangle dans un VBO (Vertex Buffer Object)
    struct attributes triangle_attributes[] = {
      {{ 0.0,  0.8, 0.0}, {1.0, 1.0, 0.0}},
      {{-0.8, -0.8, 0.0}, {0.0, 0.0, 1.0}},
      {{ 0.8, -0.8, 0.0}, {1.0, 0.0, 0.0}}
    };

    // Un VBO
    glGenBuffers(1, &_vbo_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_triangle);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_attributes), 
		 triangle_attributes, GL_STATIC_DRAW);
    // Qu'on peut rendre inactif avec
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 1;
  }
  
  // ******************************************************************** RENDER
  void display_cbk()
  {
    // Calcul les transformations qui dépendent du temps
    float move = (float) sin( _elapsed.count() * (2*3.14) / 5); // -1<->+1 every 5 seconds
    float angle = (float) _elapsed.count() * M_PI / 4.0f;  // 45° per second
    glm::vec3 axis_z(0,0,1);
    glm::mat4 m_transform = glm::translate(glm::mat4(1.0f), glm::vec3(move, 0.0, 0.0))
      * glm::rotate(glm::mat4(1.0f), angle, axis_z);
    //m_transform = glm::mat4(1.0f);
    // std::cout << "__M_TRANSFORM = " << std::endl;
    // std::cout << glm::to_string(m_transform) << std::endl;

    /* Clear the background as white */
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
 
    glUseProgram(_program);
    
    // Triangle 
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_triangle );
    glEnableVertexAttribArray(_attribute_coord3d);
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
      _attribute_coord3d, // attribute
      3,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      sizeof(struct attributes),  // stride
      0                  // offset of first element
			  );
    // Color
    glEnableVertexAttribArray(_attribute_v_color);
    glVertexAttribPointer(
      _attribute_v_color, // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      sizeof(struct attributes),  // stride
      (GLvoid*) offsetof(struct attributes, v_color)  // offset
			  );
   
    // Fade
    glUniform1f( _uniform_fade, 0.9 );
    glUniformMatrix4fv(_uniform_m_transform, 1, GL_FALSE,
     		       glm::value_ptr(m_transform));
 
    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(_attribute_coord3d);
    glDisableVertexAttribArray(_attribute_v_color);
  }
  
  void render()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Stocke l'instant de début
    auto start = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(_window)) {
      // float ratio;
      // int width, height;
      
      // glfwGetFramebufferSize(_window, &width, &height);
      // ratio = width / (float) height;
      
      // glViewport(0, 0, width, height);
      // glClear(GL_COLOR_BUFFER_BIT);
      
      // glMatrixMode(GL_PROJECTION);
      // glLoadIdentity();
      // glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      // glMatrixMode(GL_MODELVIEW);
      // glLoadIdentity();
      
      _elapsed = std::chrono::steady_clock::now() - start;
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
  GLint _attribute_coord3d, _attribute_v_color;
  /** Uniform var */
  GLint _uniform_fade, _uniform_m_transform;
  /** Vertex Buffer Object avec des triangles */
  GLuint _vbo_triangle;
  /** Buffer pour lire des shaders */
  char* _buffer;
  /** Timer en s */
  std::chrono::duration<float> _elapsed;
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
