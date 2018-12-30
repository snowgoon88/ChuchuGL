/* -*- coding: utf-8 -*- */

#ifndef GL_OVERLAY_HPP
#define GL_OVERLAY_HPP

/** 
 * Manage overlay text and position for a cell/object.
 *
 * BEWARE : GLTextShaders assume a screen of -1,1 x -1,1
 *
 * // TODO: give origin position and compute
 *    - frame around origin
 *    - text BBox and Position
 */
#include <gl_text_shaders.hpp>

// ******************************************************************** DEFINE
// Compute from level coordinate to text coordinate
#define OVERLAY_X  ((10.1 - -3.0) / (15.0 - -3.0) * (1.0 - -1.0) - 1.0 )
#define OVERLAY_Y  ((10.0 - -3.0) / (11.0 - -3.0) * (1.0 - -1.0) - 1.0 )
//#define OVERLAY_Z   0.5
// ******************************************************************* TYPEDEF
using Color = struct {
    double r,g,b;
};
using OverlayMsg = struct omsg {
  omsg( double ox, double oy, std::string msg, Color col ) :
    orix(ox), oriy(oy), msg(msg), col(col), textx(0.0), texty(0.0)
  {};
  double orix, oriy;
  std::string msg;
  Color col = {0.0, 0.0, 0.0};
  double textx, texty;
};

// ***************************************************************************
// ***************************************************************** GLOverlay
// ***************************************************************************
class GLOverlay
{
public:
  // ************************************************** GLOverlay::constructor
  GLOverlay( double x, double y, std::string msg, Color col ) :
    _data(x,y,msg, col)
  {
    _gltext.set_size( 12 );
    update_position();
  }
  virtual ~GLOverlay()
  {
  }
  // ******************************************************* GLOverlay::update
  /** Compute position given original position, game screen geometry */
  void update_position()
  {
    _data.textx = OVERLAY_X;
    _data.texty = OVERLAY_Y;
  }
  // ******************************************************* GLOverlay::render
  void render( float screen_ratio_x = 1.0, float screen_ratio_y = 1.0 )
  {
    _gltext.pre_render();
    _gltext.set_color( {_data.col.r, _data.col.g, _data.col.b, 1.0} );
    _gltext.set_scale( screen_ratio_x, screen_ratio_y );
    _gltext.render( _data.msg, _data.textx, _data.texty );    
    _gltext.post_render();
  }
  // **************************************************** GLOverlay::attributs
  OverlayMsg _data;
  GLTextShaders _gltext;
}; // GLOverlay

#endif // GL_OVERLAY_HPP
