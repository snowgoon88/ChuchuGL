/* -*- coding: utf-8 -*- */

#ifndef GL_3DENGINE_HPP
#define GL_3DENGINE_HPP

/** 
 * Crée une fenêtre GLFW où on peut dessiner avec les différents
 * shaders initialisé dans ce moteur.
 * Un GLScreen peut être lancé.
 * TODO Voir glconcept::GLScreen.
 */

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#include <memory>                        // std::*_ptr
// Viewer
#include <gl_texture.hpp>
#include <gl_texture_fade.hpp>
#include <gl_3dunicolor.hpp>

// ******************************************************************** GLOBAL
typedef std::unique_ptr<GLTexture> GLTexturePtr;
typedef std::unique_ptr<GLTextureFade> GLTextureFadePtr;
using GL3DUnicolorPtr = std::unique_ptr<GL3DUnicolor>;
// ***************************************************************************
// **************************************************************** GL3DEngine
// ***************************************************************************
class GL3DEngine
{
public:
  // **************************************************** GL3DEngine::creation
  GL3DEngine(const std::string& title = "GL Engine",
	   int width=800, int height=600,
	   bool fullsize=false) :
    _screen_width(width), _screen_height(height),
    _gl_texture(nullptr), _gl_texture_fade(nullptr),
	_gl_unicolor(nullptr)
  {
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
      exit(EXIT_FAILURE);

    if( fullsize ) {
      _window = glfwCreateWindow(width, height, title.c_str(),
				 glfwGetPrimaryMonitor(), NULL);
    }
    else {
      _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    }
    
    if (!_window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    // 'this' est associé à cette _window (pour les callback)
    glfwSetWindowUserPointer( _window, this);
    glfwMakeContextCurrent(_window);

    // Shaders
    _gl_texture = GLTexturePtr(new GLTexture("src/shaders/sprite") );
    _gl_texture_fade = GLTextureFadePtr(new GLTextureFade("src/shaders/sprite_fade") );
	_gl_unicolor = GL3DUnicolorPtr(new GL3DUnicolor("src/shaders/line3d"));
  }
  // *************************************************** GL3DEngine::destruction
  ~GL3DEngine()
  {
    glfwDestroyWindow(_window);
    glfwTerminate();
  };
  // *********************************************************** GL3DEngine::run
  /** 
   * Comportement par défaut : fenêtre blanche.
   * Arrêt avec ESC */
  void run()
  {
    // set key callback
    glfwSetKeyCallback(_window, key_callback);
    
    // render white window
    while (!glfwWindowShouldClose(_window)) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as white */
      glClearColor(1.f, 1.f, 1.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  };
  /**
   * Function pour lancer un concept::GLScreen.
   */
  template<class Screen, typename Result>
  Result run( Screen& screen )
  {
    screen.init();
    screen.render();
    
    return screen.final_state();
  }
  // ***************************************************************** attributs
  const GLTexture& gl_texture() const { return *_gl_texture; };
  const GLTextureFade& gl_texture_fade() const { return *_gl_texture_fade; };
  const GL3DUnicolor& gl_unicolor() const { return *_gl_unicolor; }
  GLFWwindow* window() { return _window; };
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  /** Shaders */
  GLTexturePtr     _gl_texture;
  GLTextureFadePtr _gl_texture_fade;
  GL3DUnicolorPtr _gl_unicolor;
  // ****************************************************** GL3DEngine::callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  };
  /**
   * callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
};


#endif // GL_3DENGINE_HPP
