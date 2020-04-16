/* -*- coding: utf-8 -*- */

#ifndef GL_3DENGINE_HPP
#define GL_3DENGINE_HPP

/** 
 * Create a GLFW _window where one can draw with the various
 * shaders intialized.
 * Can run a GLScreen (TODO see glconcept::GLScreen)
 *
 * Shaders, tu be used with
 *    ATT: glEnableVertexAttribArray and glVertexAttribPointer
 *    UNI: glUniformMatrix4fv/1f/ ( ... )
 * _gl_unicolor : 
 *    ATT: vec3 coord3d
 *    UNI: mat4 mvp, vec3 l_color, float fade
 * _gl_multicolor :
 *    ATT: vec3 coord3d, vec3 v_color
 *    UNI: mat4 mvp, float fade
 *
 *
 * SINGLETON : cannot be copied or assigned !
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
#include <gl_3dmulticolor.hpp>
#include <gl_3dtext_shaders.hpp>


// ******************************************************************** GLOBAL
class GL3DEngine;
using GL3DEnginePtr = std::shared_ptr<GL3DEngine>;
using GLTexturePtr = std::shared_ptr<GLTexture>;
using GLTextureFadePtr = std::shared_ptr<GLTextureFade>;
using GL3DUnicolorPtr = std::shared_ptr<GL3DUnicolor>;
using GL3DMulticolorPtr = std::shared_ptr<GL3DMulticolor>;
using GL3DTextPtr = std::shared_ptr<GL3DTextShaders>;
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
	_gl_unicolor(nullptr), _gl_text(nullptr)
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
    // 'this' is associated with this _window (for callback)
    glfwSetWindowUserPointer( _window, this);
    glfwMakeContextCurrent(_window);

    // Shaders
    _gl_texture = GLTexturePtr (new GLTexture("src/shaders/sprite") );
    _gl_texture_fade = GLTextureFadePtr(new GLTextureFade("src/shaders/sprite_fade") );
    _gl_unicolor = GL3DUnicolorPtr(new GL3DUnicolor("src/shaders/line3d"));
    _gl_multicolor = GL3DMulticolorPtr(new GL3DMulticolor("src/shaders/line_tri_xyz_rgb_fade"));
    _gl_text = GL3DTextPtr(new GL3DTextShaders() );
  }
  /** copy creation */
  GL3DEngine( const GL3DEngine& eng ) = delete;
  // 	_screen_width(eng._screen_width), _screen_height(eng._screen_height),
  //   _gl_texture(eng._gl_texture), _gl_texture_fade(eng._gl_texture_fade),
  // 	_gl_unicolor(eng._gl_unicolor), _gl_text(eng._gl_text)
  // {}
  /** assignment creation */
  GL3DEngine operator=( const GL3DEngine& eng ) = delete;
  // {
  //   if (this != &eng) { // protect against invalid self-assignment
  // 	  _screen_width = eng._screen_width;
  // 	  _screen_height = eng._screen_height;
  // 	  _gl_texture = eng._gl_texture;
  // 	  _gl_texture_fade = eng._gl_texture_fade;
  // 	  _gl_unicolor = eng._gl_unicolor;
  // 	  _gl_text = eng._gl_text;
  //   }
  //   return *this;
  // }
  // *************************************************** GL3DEngine::destruction
  ~GL3DEngine()
  {
    glfwDestroyWindow(_window);
    glfwTerminate();
  }
  // *********************************************************** GL3DEngine::run
  /** 
   * Default behavior : white window.
   * stop avec ESC */
  void run()
  {
    // set key callback
    glfwSetKeyCallback(_window, key_callback);
    
    // render white window
    while (!glfwWindowShouldClose(_window)) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Check what glViewport do.
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as white */
      glClearColor(1.f, 1.f, 1.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


      glfwSwapBuffers(_window);
      glfwPollEvents();
    }
  }
  /**
   * To run a concept::GLScreen
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
  const GL3DMulticolor& gl_multicolor() const { return *_gl_multicolor; }
  GL3DTextShaders& gl_text() const { return *_gl_text; }
  GLFWwindow* window() { return _window; };
private:
  /** Ptr to the Window */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  /** Shaders */
  GLTexturePtr    _gl_texture;
  GLTextureFadePtr _gl_texture_fade;
  GL3DUnicolorPtr  _gl_unicolor;
  GL3DMulticolorPtr _gl_multicolor;
  GL3DTextPtr      _gl_text;
  // ****************************************************** GL3DEngine::callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }
  /**
   * callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
};


#endif // GL_3DENGINE_HPP
