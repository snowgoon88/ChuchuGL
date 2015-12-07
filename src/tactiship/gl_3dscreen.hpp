/* -*- coding: utf-8 -*- */

#ifndef GL_3DSCREEN_HPP
#define GL_3DSCREEN_HPP

/** 
 * TODO
 */

#include<gl_3dvect.hpp>

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>

class GL3DScreen
{
public:
  // **************************************************** GL3DScreen::creation
  GL3DScreen( GLEngine& engine ) :
    _window(engine.window()),
    _finished(false)
  {
  };
  // ******************************************************** GL3DScreen::init
  /** Callback pour touches et souris */
  void init() 
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    // glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback( _window, mouse_button_callback );
    glfwSetMousePosCallback( _window, mouse_move_callback );
  };
  // ********************************************************* GLScreen::render
  /**
   * render() est bloquant, ne rendant la main que quand le GLSCreen 
   * est terminé.
   * Pas de surprise : gère l'affichage en utilisant OpenGL.
   */
  void render () 
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (!glfwWindowShouldClose(_window) and not _finished) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as white */
      glClearColor(1., 1., 1., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Projection to screen
      glm::mat4 projection = glm::ortho( -3.f, 3.f, // left;right
					 -3.f, 3.f, // bottom,top
					 -3.f, 3.f // near far
					 );
      
      // _gl_vect.pre_render();
      _gl_vect.render( projection );
      // _gl_vect.post_render();
      // Remove any programm so that glText can "work"
      glUseProgram(0);

      glfwSwapBuffers(_window);
      glfwPollEvents();

    }
  };
  // ************************************************** GL3DScreen::final_state
  bool final_state() const { return _finished; };
  // *************************************************** GL3DScreen::attributs
  GLFWwindow* window() { return _window; };
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width=800, _screen_height=600;
  /** ready */
  bool _finished;
  /** Viewer */
  GL3DVect _gl_vect;
};

#endif // GL_3DSCREEN_HPP
