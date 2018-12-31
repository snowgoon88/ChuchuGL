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
#include <gl_shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <list>

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
using OverlayMsg = struct om {
  std::list<OverlayLine> lines;
  double xori, yori;
  Color col;
};

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
  GLOverlay()
  {
    // Basic shaders
    _line_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                                 "src/shaders/base3D.frag330.glsl" );
    _proj_view_loc_line = _line_shader->getUniformLocation( "proj_view" );
    _model_loc_line = _line_shader->getUniformLocation( "model" );
    build_vbo();
    
    // Font size
    _gltext.set_size( 12 );

    // No messages for the moment
    _textlines.clear();
  }
  virtual ~GLOverlay()
  {
    glDeleteVertexArrays( 1, &_line_vao);
    glDeleteBuffers( 1, &_line_vbo);
    delete _line_shader;
  }
  // ***************************************************** GLOverlay::add_text
  void add_text( const std::string& msg, const Color& col )
  {
    _textlines.push_back( {msg, col} );
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
  void update_geom()
  {
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

    _line_vtx.clear();
    // add first vertical point, red
    _line_vtx.push_back( overlay2projview( textx - 0.01,
                                           texty + 1.5 * _gltext.line_height()) );
    for( auto& l: _textlines) {
      _gltext.set_color( {l.col.r, l.col.g, l.col.b, 1.0} );
      _gltext.render( l.msg, textx, texty );
      texty -= 1.5 * _gltext.line_height();
    }
    // Add last vertical point, red
    _line_vtx.push_back( overlay2projview( textx - 0.01,
                                           texty + 0.5*_gltext.line_height()  ));
    // Again, to start horizontal, red
    _line_vtx.push_back( overlay2projview( textx - 0.01,
                                           texty + 0.5*_gltext.line_height()  ));
    // And an horizontal end, red
    _line_vtx.push_back( overlay2projview( textx + 0.1,
                                           texty + 0.5*_gltext.line_height()  ));
    
    _gltext.post_render();

    // DEBUG
    // std::cout << "__Overlay " << std::endl;
    // for( auto& vtx: _line_vtx) {
    //   std::cout << "vtx=" << vtx.x << ", " << vtx.y << ", " << vtx.z;
    //   std::cout << "  col=" << vtx.r << ", " << vtx.g << ", " << vtx.b << std::endl;
    // }

    // dynamic VBA and VBO
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
  /** Vertex in projview coordinate (red by default) */
  Vertex overlay2projview( double x, double y )
  {
    Vertex vtx {
      float(((x - -1.0) / (1.0 - -1.0)) * (15.0 - -3.0) + -3.0),
        float(((y - -1.0) / (1.0 - -1.0)) * (11.0 - -3.0) + -3.0),
        0.1f,
        1.f, 0.f, 0.f };
    return vtx;
  }
  
  // **************************************************** GLOverlay::attributs
  GLTextShaders _gltext;
  std::list<OverlayLine> _textlines;
  std::vector<Vertex> _line_vtx;
  GLShader* _line_shader;
  GLuint _proj_view_loc_line;
  GLuint _model_loc_line;
  GLuint _line_vao, _line_vbo;
  
}; // GLOverlay

#endif // GL_OVERLAY_HPP
