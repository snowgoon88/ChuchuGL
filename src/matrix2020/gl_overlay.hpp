/* -*- coding: utf-8 -*- */

#ifndef GL_OVERLAY_HPP
#define GL_OVERLAY_HPP

/** 
 * Manage overlay text and position for a cell/object.
 *
 * A list of OverlayMsgs with : 
 *  - cell of origin
 *  - frame color
 *  - multiline text as list of OverLine
 *
 * Square is drawn around origin cell, Frame around overlays
 * and text message. Link between origin cell and frame.
 *
 * When an update is requested, a prior order of overlay is computed,
 * then precised in order to avoid CROSSING of links (crossing is BAD).
 * TODO : frame position is computed twice...
 *
 * BEWARE : GLTextShaders assume a screen of -1,1 x -1,1
 */
#include <gl_text_shaders.hpp>
#include <gl_shader.hpp>

#include <key_pressed.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <list>
#include <vector>
#include <memory>     // shared_ptr
#include <math.h>     // fabs
#include <iterator>   // std::next, std::prev
#include <algorithm>  // std::rotate
// ******************************************************************** DEFINE
// Compute from level coordinate to text coordinate
#define OVERLAY_X  ((10.1 - -3.0) / (15.0 - -3.0) * (1.0 - -1.0) - 1.0 )
#define OVERLAY_Y  ((10.0 - -3.0) / (11.0 - -3.0) * (1.0 - -1.0) - 1.0 )
//#define OVERLAY_Z   0.5
// ******************************************************************* TYPEDEF
using Color = struct {
    double r,g,b;
};
using OverlayLine = struct ol {
  ol( std::string msg, Color col ) :
    msg(msg), col(col) 
  {};
  std::string msg;
  Color col = {0.0, 0.0, 0.0};
};

// ***************************************************************************
// **************************************************************** OverlayMsg
// ***************************************************************************
class OverlayMsg
{
public:
  // ************************************************* OverlayMsg::constructor
  OverlayMsg( double xori = 0.0, double yori = 0.0, Color col = {1.0,0.0,0.0}):
    _xori(xori), _yori(yori), _col(col)
  {
    _textlines.clear();
  }
  virtual ~OverlayMsg()
  {
  }
  // ***************************************************** OverlayMsg::display
  std::string str_dump() const
  {
    std::stringstream dump;

    dump << "OV [" << _textlines.front().msg << "] ";
    dump << "ori = " << _xori+0.5 << ", " << _yori << " ";
    dump << "frame = " << OVERLAY_X - 0.01 << " | " << ytop << ", " << ylink << ", " << ybottom;

    return dump.str();
  }
  // ******************************************************** OverlayMsg::text
  void add_text( const std::string& msg, const Color& col )
  {
    _textlines.push_back( {msg, col} );
  }
  // ****************************************************** OverlayMsg::update
  void update_origin( double xori, double yori )
  {
    _xori = xori;
    _yori = yori;
  }
  void update_color( Color col )
  {
    _col = col;
  }
  // *************************************************** OverlayMsg::attributs
  double _xori, _yori;
  Color _col;
  std::list<OverlayLine> _textlines;
  // where the link will be attached, in screen coordinate
  double ylink;
  // frame, in screen coordinate
  double ytop, ybottom;

}; // OverlayMsg
// ******************************************************* OverlayMsg::compare
/** bottom right is smaller */
inline bool operator<(const OverlayMsg& lhs, const OverlayMsg& rhs)
{
  if (lhs._yori < rhs._yori)
    return true;
  if (lhs._yori > rhs._yori)
    return false;
  return (lhs._xori > rhs._xori );
}
inline bool operator> (const OverlayMsg& lhs, const OverlayMsg& rhs)
{return  operator< (rhs,lhs);}
inline bool operator<=(const OverlayMsg& lhs, const OverlayMsg& rhs)
{return !operator> (lhs,rhs);}
inline bool operator>=(const OverlayMsg& lhs, const OverlayMsg& rhs)
{return !operator< (lhs,rhs);}
// ***************************************************************************
using OverlayMsgIte = std::list<OverlayMsg>::iterator;

// ***************************************************************************
// ***************************************************************** GLOverlay
// ***************************************************************************
class GLOverlay
{
  using Vertex = struct s_Vertex {
    GLfloat x, y, z; // pos
    GLfloat r, g, b; // color
  };
private:
  // max number of lines to draw
  static const unsigned int _max_size = 100;
public:
  // ************************************************** GLOverlay::constructor
  GLOverlay() : _fg_need_update(false)
  {
    // Basic shaders
    _line_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                                 "src/shaders/base3D.frag330.glsl" );
    _proj_view_loc_line = _line_shader->getUniformLocation( "proj_view" );
    _model_loc_line = _line_shader->getUniformLocation( "model" );
    build_vbo();
    
    // Font size
    _gltext.set_size( 12 );

    // No overlays
    _overlays.clear();
    // No messages for the moment
    _textlines.clear();
  }
  virtual ~GLOverlay()
  {
    glDeleteVertexArrays( 1, &_line_vao);
    glDeleteBuffers( 1, &_line_vbo);
    delete _line_shader;
  }
  // ****************************************************** GLOverlay::display
  std::string str_dump() const
  {
    std::stringstream dump;

    dump << "__OVERLAY" << std::endl;
    for( auto& omsg: _overlays) {
      dump << "  " << omsg.str_dump() << std::endl;
    }

    return dump.str();
  }
  // ********************************************************** GLOverlay::add
  OverlayMsgIte add_overlay( double xori = 0.0, double yori = 0.0,
                             Color col = {1.0,0.0,0.0})
  {
    auto it = _overlays.insert( _overlays.end(),
                                OverlayMsg( xori, yori, col ));
    update();
    return it;
  }
  // ******************************************************** GLOverlay::vbo
  void build_vbo()
  {
    _line_vtx.clear();
    
    glGenVertexArrays( 1, &_line_vao );
    glGenBuffers( 1, &_line_vbo );
    glBindVertexArray( _line_vao );

    glBindBuffer( GL_ARRAY_BUFFER, _line_vbo );
    // glBufferData( GL_ARRAY_BUFFER, sizeof(square_vtx), square_vtx,
    //               GL_STATIC_DRAW );
    glBufferData( GL_ARRAY_BUFFER,
                  GLOverlay::_max_size * sizeof(Vertex),
                  NULL, //not _line_vtx.data() as it might be empty
                  GL_DYNAMIC_DRAW );

    // position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray(0);
    // color coordinate
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray(1);
  }
  // ******************************************************* GLOverlay::update
  void update()
  {
    // ask for update
    _fg_need_update = true;
  }
  void update_geom()
  {
    //std::cout << "__SORT" << std::endl;
    //std::cout << str_dump()  << std::endl;
    // Prior, reorder with top left bigger
    _overlays.sort();
    _overlays.reverse();
    //std::cout << str_dump()  << std::endl;

    // Now fine tuning of order
    double ycur = OVERLAY_Y;
    auto it = _overlays.begin();
    // update frame info
    it->ylink = ycur;
    it->ytop = ycur + 1.5 * _gltext.line_height();
    ycur -= 1.5 * double(it->_textlines.size()) * _gltext.line_height();
    it->ybottom = ycur + 0.5*_gltext.line_height();
    
    auto itnext = std::next( it );

    while( itnext != _overlays.end() ) {
      // update frame info
      itnext->ylink = ycur;
      itnext->ytop = ycur + 1.5 * _gltext.line_height();
      itnext->ybottom = ycur - 1.5 * double(it->_textlines.size()) * _gltext.line_height() + 0.5 * _gltext.line_height();

      // std::cout << "++ COMPARE" << std::endl;
      // std::cout << str_dump() << std::endl;
      // std::cout << "  it = " << it->str_dump() << std::endl;
      // std::cout << "  ne = " << itnext->str_dump() << std::endl;
      // std::cout << "  yc = " << ycur << std::endl;

      // if intersection, reorder
      // ycur, ylink, OVERLAY_X are all in overlay coordinates
      // xori, yori are in projview coordinate
      // interesction computation must be done with same coordinates system
      auto cross = intersect( it->_xori+0.5, it->_yori,
                              x_fromoverlay(OVERLAY_X - 0.01), y_fromoverlay(it->ylink),
                              itnext->_xori+0.5, itnext->_yori,
                              x_fromoverlay(OVERLAY_X - 0.01), y_fromoverlay(itnext->ylink) );
      if (cross) {
        //std::cout << "  !! INTERESECT" << std::endl;
        // move itnext before it
        _overlays.splice( it, _overlays, itnext );
        //std::cout << str_dump() << std::endl;
        
        // update things
        if (itnext == _overlays.begin() ) {
          //std::cout << "    + become first" << std::endl;
          ycur = OVERLAY_Y;
          it = itnext;
          // update frame info
          it->ylink = ycur;
          it->ytop = ycur + 1.5 * _gltext.line_height();
          ycur -= 1.5 * double(it->_textlines.size()) * _gltext.line_height();
          it->ybottom = ycur + 0.5*_gltext.line_height();
          
          itnext = std::next( it );
        }
        else {
          //std::cout << "    + simple reorder" << std::endl;
          it = std::prev( itnext );
          ycur = it->ybottom - 0.5*_gltext.line_height();
        }
      }
      else { // not crossing, keep going
        //std::cout << "  OK, not crossing" << std::endl;
        ycur = itnext->ybottom - 0.5*_gltext.line_height();
        it = itnext;
        itnext = std::next( it );
      }
    }
    // and itnext will be checked again
    //wait_keypressed();

    _fg_need_update = false;
  }
  // ******************************************************* GLOverlay::render
  void render( const glm::mat4& projview,
               float screen_ratio_x = 1.0, float screen_ratio_y = 1.0 )
  {
    // Transparence
    // Enable alpha
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    double textx = OVERLAY_X;
    double texty = OVERLAY_Y;
    
    _gltext.pre_render();
    _gltext.set_scale( screen_ratio_x, screen_ratio_y );

    if (_fg_need_update) {
      update_geom();
    }
    
    _line_vtx.clear();
    // Render all OverlayMsg texts while computing line vtx
    // TODO: frame coordinates are already computed in _overlays
    //       no need to do that again !!
    for( auto& ovr: _overlays) {
      // add square around ori position of Overlay
      add_square( ovr._xori, ovr._yori, ovr._col );
      // and a vtx at the begining of frame
      _line_vtx.push_back( vtx_from_overlay( textx - 0.01,
                                             texty,
                                             ovr._col) );
      
      // add first vertical point, Color of ovr
      _line_vtx.push_back( vtx_from_overlay( textx - 0.01,
                                          texty + 1.5 * _gltext.line_height(),
                                          ovr._col) );
      for( auto& l: ovr._textlines) {
        _gltext.set_color( {l.col.r, l.col.g, l.col.b, 1.0} );
        _gltext.render( l.msg, textx, texty );
        texty -= 1.5 * _gltext.line_height();
      }
      // Add last vertical point, Color of ovr
      _line_vtx.push_back( vtx_from_overlay( textx - 0.01,
                                             texty + 0.5*_gltext.line_height(),
                                             ovr._col) );
      // Again, to start horizontal, Color of ovr
      _line_vtx.push_back( vtx_from_overlay( textx - 0.01,
                                             texty + 0.5*_gltext.line_height(),
                                             ovr._col) );
      // And an horizontal end, Color of ovr
      _line_vtx.push_back( vtx_from_overlay( textx + 0.1,
                                             texty + 0.5*_gltext.line_height(),
                                             ovr._col) );

      // increase a bit texty
      texty -= 1.5 * _gltext.line_height();
    }
    
    _gltext.post_render();

    // DEBUG
    // std::cout << "__Overlay " << std::endl;
    // for( auto& vtx: _line_vtx) {
    //   std::cout << "vtx=" << vtx.x << ", " << vtx.y << ", " << vtx.z;
    //   std::cout << "  col=" << vtx.r << ", " << vtx.g << ", " << vtx.b << std::endl;
    // }
    // std::cout << str_dump() << std::endl;

    // Now the lines using a dynamic VBA and VBO
    // specify the buffer we are about to update
    glBindBuffer( GL_ARRAY_BUFFER, _line_vbo );
    // ask for reallocation, glBufferData with NULL and same parameters
    // see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming)
    glBufferData( GL_ARRAY_BUFFER,
                  GLOverlay::_max_size * sizeof(Vertex),
                  NULL, 
                  GL_DYNAMIC_DRAW );
    // will call glBufferSubData on the entire buffer
    glBufferSubData( GL_ARRAY_BUFFER,
                     0,                                   //start of sub
                     _line_vtx.size() * sizeof(Vertex), //length sub
                     _line_vtx.data() );                // data
    
    // now render lines
    _line_shader->use();
    // create transformations
    glm::mat4 model( 1.0f );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_line, 1, GL_FALSE,
                        glm::value_ptr( projview ) );
    glUniformMatrix4fv( _model_loc_line, 1, GL_FALSE,
                        glm::value_ptr(model) );

    glBindVertexArray(_line_vao);
    glDrawArrays(GL_LINES, 0, _line_vtx.size() ); // mode, first, count
  }
  // **************************************************** GLOverlay::transform
  /** vtx coordinate from overlay */
  double x_fromoverlay( double overx )
  {
    return ((overx - -1.0) / (1.0 - -1.0)) * (15.0 - -3.0) + -3.0;
  }
  double y_fromoverlay( double overy )
  {
    return ((overy - -1.0) / (1.0 - -1.0)) * (11.0 - -3.0) + -3.0;
  }
  /** Vertex in projview coordinate (red by default) */
  Vertex vtx_from_overlay( double x, double y,
                           Color col = {1.0, 0.0, 0.0} )
  {
    Vertex vtx {
        float( x_fromoverlay(x) ),
        float( y_fromoverlay(y) ),
        0.1f,
        float(col.r), float(col.g), float(col.b) };
    return vtx;
  }
  // *************************************************************************
  // **************************************************** GLOverlay::attributs
  GLTextShaders _gltext;
  std::list<OverlayMsg> _overlays;
  std::list<OverlayLine> _textlines;
  std::vector<Vertex> _line_vtx;
  GLShader* _line_shader;
  GLuint _proj_view_loc_line;
  GLuint _model_loc_line;
  GLuint _line_vao, _line_vbo;
  bool _fg_need_update;
  
private:
  // *************************************************** GLOverlay::add_square
  void add_square( double xori, double yori,
                   Color col = {1.0, 0.0, 0.0} )
  {
    // (i/2) takes value 0 0 1 1
    // (i+1)/2%2 takes 0 1 1 0
    for( int i=0; i<4; ++i ) {
      int xini = (-1 + 2*(((i+1)/2)%2));
      int xend = (1 - 2*(i/2));
      int yini = (-1 + 2*(i/2));
      int yend = (-1 + 2*(((i+1)/2)%2));

      add_point( xori + double(xini) * 0.5,
                 yori + double(yini) * 0.5,
                 col );
      add_point( xori + double(xend) * 0.5,
                 yori + double(yend) * 0.5,
                 col );
    }
    // and a vtx at half of right side
    add_point( xori + 0.5, yori, col );
  }
  void add_point( double xori, double yori,
                   Color col = {1.0, 0.0, 0.0} )
  {
    _line_vtx.push_back( { float(xori), float(yori), 0.1f,
          float(col.r), float(col.g), float(col.b) } );
  }
  // **************************************************** GLOverlay::intersect
  /** 
   * test if segments p and q intersects
   * https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
   */
  bool intersect( double pstartx, double pstarty, double pendx, double pendy,
                  double qstartx, double qstarty, double qendx, double qendy )
  {
        
    double px = pstartx;
    double py = pstarty;
    double rx = pendx - pstartx;
    double ry = pendy - pstarty;

    double qx = qstartx;
    double qy = qstarty;
    double sx = qendx - qstartx;
    double sy = qendy - qstarty;

    double r_s = rx * sy - ry * sx;
    double qmp_s = ((qx-px) * sy - (qy-py) * sx);;
    double qmp_r = (qx-px) * ry - (qy-py) * rx;
    
    // r_s == 0 and qmp_r == O : colinear, BUT CANNOT HAPPEND HERE
    // r_s == 0 : parallel : OK, not important
    // r_s != 0 and 0<=t<=1 and 0<=u<=1 : intersect TRUE
    // BEWARE : Here we consider that t==0 and u==0 is NOT a crossing !!!!!!!!
    if ( fabs(r_s) > 0.0000000001 ) {
      double t = qmp_s / r_s;
      double u = qmp_r / r_s;
      if ( (0 < t) and (t <= 1.0) and (0 < u) and (u <= 1) ) {
        return true;
      }
    }
    // otherwise, no intersection
    return false;
  }

}; // GLOverlay

#endif // GL_OVERLAY_HPP
