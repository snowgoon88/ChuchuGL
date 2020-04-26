/* -*- coding: utf-8 -*- */

#ifndef GL_3DCANVASSCREEN_HPP
#define GL_3DCANVASSCREEN_HPP

/** 
 * GL3DScreen with mouse events propagated to GL3DCanvas::Trackball.
 * 'r/R' key does also reset the GL3DCanvas::Trackball.
 *
 * Children should (at least) call GL3DSCanvasScreen::key_callbak( ... ).
 */
#include <3d/gl_3dscreen.hpp>

class GL3DCanvasScreen : public GL3DScreen
{
public:
  // ********************************************** GL3DCanvasScreen::creation
  GL3DCanvasScreen( GL3DCanvas& canvas ) :
    GL3DScreen(),
    _canvas( canvas )
  {}
  // ****************************************************** GL3DScreen::render
  virtual void render( int win_width, int win_height )
  {}
  // *************************************************** GL3DScreen::callbacks
  virtual void key_callback(int key, int scancode, int action, int mods)
  {
    // R
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      _canvas.reset_trackball();
    }
  }
  virtual void mouse_button_callback( int button, int action, int mods,
                                      double xpos, double ypos,
                                      int win_width, int win_height )
  {
    _canvas.mouse_button_trackball( button, action, mods,
                                    xpos/win_width, ypos/win_height );
  }
  virtual void mouse_move_callback( double xpos, double ypos,
                                    int win_width, int win_height )
  {
     _canvas.mouse_move_trackball( xpos/win_width, ypos/win_height );
  }
  virtual void mouse_scroll_callback( double xoffset, double yoffset )
  {
    _canvas.mouse_scroll_trackball( xoffset, yoffset );
  }
protected:
  // ********************************************* GL3DCanvasScreen::attributs
  GL3DCanvas& _canvas;
}; // GL3DCanvasScreen

#endif // GL_3DCANVASSCREEN_HPP

