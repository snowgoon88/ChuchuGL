/* -*- coding: utf-8 -*- */

#ifndef GL_3DSCREEN_HPP
#define GL_3DSCREEN_HPP

/** 
 * GL3DScreen : skeleton.
 */

class GL3DScreen
{
public:
  GL3DScreen () {}
  virtual ~GL3DScreen() {}
  // ****************************************************** GL3DScreen::render
  virtual void render( int win_width, int win_height )
  {
    std::cout << "__RENDER screen" << std::endl;
  }
  // *************************************************** GL3DScreen::callbacks
  virtual void key_callback(int key, int scancode, int action, int mods)
  {}
  virtual void mouse_button_callback( int button, int action, int mods,
                                      double xpos, double ypos,
                                      int win_width, int win_height )
  {}
  virtual void mouse_move_callback( double xpos, double ypos,
                                    int win_width, int win_height )
  {}
  virtual void mouse_scroll_callback( double xoffset, double yoffset )
  {}
}; // GL3DScreen

#endif // GL_3DSCREEN_HPP

