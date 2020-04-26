/* -*- coding: utf-8 -*- */

#ifndef GL_3DCANVAS_HPP
#define GL_3DCANVAS_HPP

/** 
 * Offer some basic Shaders to facilitate drawing with OpenGL
 * on a window.
 *
 * - _smart_shader => uniform or attrib color + fade
 * - _text_shader  => 
 *         set_scale for the TextShader
 *
 * A Trackball (from VisuGL)
 *
 * With the SmartShader, mode indicates if:
 * - 0 : use the color of the vortex
 * - 1 : use uniform color (_u_color)
 * one can also specify a fade parameter.
 *
 * @see 'solar_screen' as a working example.
 */
#include <vector>

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// TODO: should remove the GLFW (needed by trackball !!!)
#include <GLFW/glfw3.h>
#include <visugl/trackball.hpp>

#include <gl_shader.hpp>
#include <3d/gl_3dtext_shaders.hpp>

// ***************************************************************************
// **************************************************************** GL3DCanvas
// ***************************************************************************
class GL3DCanvas
{
  using ColorRGB = std::vector<GLfloat>;
  using ColorRGBA = std::vector<GLfloat>;
  using VTXarray = std::vector<GLfloat>;
  using IDXarray = std::vector<GLuint>;
public:
  // ************************************************* GL3DCanvas::constructor
  GL3DCanvas() :
    _trackball(),
    _color_bg( {0.f, 0.f, 0.f} ), // black background
    _text_scale( 5.f )
  {
    // Smart Shaders and input variables locations
    _smart_shader = new GLShader( "src/shaders/m_vp_pos_col_fade_mode.v330.glsl",
                                  "src/shaders/m_col.f330.glsl" );
    _u_mode = _smart_shader->getUniformLocation( "u_mode" );
    _u_model = _smart_shader->getUniformLocation( "u_model4x4" );
    _u_pv = _smart_shader->getUniformLocation( "u_proj_view4x4" );
    _u_fade = _smart_shader->getUniformLocation( "u_fade" );
    _u_color = _smart_shader->getUniformLocation( "u_col3" );
    _a_vertex = _smart_shader->getAttribLocation( "a_pos3" );
    _a_color = _smart_shader->getAttribLocation( "a_col3" );

    // TextShader
    _text_shader = new GL3DTextShaders();
  }
  /** Fill the Viewport with background color
   */
  void render_viewport( const int posx, const int posy,
                        const int width, const int height) const
  {
    glViewport(posx, posy, width, height);
    /* Clear the background with background color */
    glClearColor( _color_bg[0], _color_bg[1], _color_bg[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  }
  // *************************************************** GL3DCanvas::trackball
  /** Transform matrix from the Trackball
   */
  const glm::mat4& get_trackball_transform() const
  {
    return _trackball.get_transform();
  }
  void reset_trackball()
  {
    _trackball.reset();
  }
  void mouse_button_trackball( int button, int action, int mods,
                               double relative_x, double relative_y )
  {
    _trackball.on_mouse_btn( button, action, mods,
                             relative_x, relative_y );
  }
  void mouse_move_trackball( double realtive_x, double relative_y )
  {
    _trackball.on_mouse_move( realtive_x, relative_y );
  }
  void mouse_scroll_trackball( double xoffset, double yoffset )
  {
    _trackball.on_mouse_scroll( xoffset, yoffset );
  }
  visugl::TrackBall& trackball()
  {
    return _trackball;
  }  
  // ****************************************** GL3DCanvas::smart_shader_param
  /** Can force to use program while executed with force_use to true
   */
  void set_param( GLint mode, GLfloat fade, bool force_use=false )
  {
    if (force_use) _smart_shader->use();
    glUniform1i( _u_mode, mode );
    glUniform1f( _u_fade, fade );
  }
  void set_color_rgb( const ColorRGB& color, bool force_use=false )
  {
    if (force_use) _smart_shader->use();
    glUniform3f( _u_color, color[0], color[1], color[2] );
  }
  /** WARNING : a GLprogram should be used
   */
  void set_projection_view_transform( const glm::mat4& pv_tf )
  {
    glUniformMatrix4fv( _u_pv, 1, GL_FALSE, glm::value_ptr(pv_tf) );
  }
  void set_model_transform( const glm::mat4& model_tf )
  {
    glUniformMatrix4fv( _u_model, 1, GL_FALSE, glm::value_ptr(model_tf) );
  }
  // **************************************************** GLD3Canvas::make_vao
  /** vtx is a vector for making faces as [3x(x,y,z, r,gb)]
   *  idx is a vector for making indices for lines as [2x(i,i+1)]
   *
   * Return id of VAO, VertexVBO and indicesVBO
   */
  void make_vao( const std::vector<GLfloat>& vtx, const std::vector<GLuint>& idx,
                 GLuint& vao, GLuint& vtx_vbo, GLuint& idx_vbo,
                 bool verb=false )
  {
    // VAO for faces and lines
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
   
    // Build VBO for vertex
    glGenBuffers(1, &vtx_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vtx_vbo);
    // push xyz+rgb into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GLfloat ) * vtx.size(),
  		 vtx.data(), GL_STATIC_DRAW);

    if (verb) {
      std::cout << "  pushed " <<  vtx.size() << " vtx into vtx_vbo" << std::endl;
    }
    // Build VBO for line indices
    glGenBuffers(1, &idx_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_vbo );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idx.size(),
  		 idx.data(), GL_STATIC_DRAW);
    
    if (verb) {
      std::cout << "  pushed " << idx.size() << " idx into idx_vbo" << std::endl;
    }

    std::cout << "__MAKE_VAO vao=" << vao;
    std::cout << " vtx_vbo=" << vtx_vbo;
    std::cout << " idx=" << idx_vbo << std::endl;
    
    // position attribute
    glVertexAttribPointer( vertex_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( vertex_attribut() );
    // texture coordinate
    glVertexAttribPointer( color_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( color_attribut() );

    // Unbind VAO
    // TODO: test without unbinding...
    glBindVertexArray( 0 );
  }
  void delete_vao( GLuint& vao, GLuint& vtx_vbo, GLuint& idx_vbo )
  {
    glDeleteBuffers( 1, &vtx_vbo );
    glDeleteBuffers( 1, &idx_vbo );
    glDeleteVertexArrays( 1, &vao );
  }
  // *************************************************** GL3DCanvas::attributs
public:
  GLuint vertex_attribut() const { return _a_vertex; }
  GLuint color_attribut() const { return _a_color; }
  GLShader* smart_shader() const { return _smart_shader; }
  GL3DTextShaders* text_shader() const { return _text_shader; }
protected:
  GLShader* _smart_shader;
  GL3DTextShaders* _text_shader;
  // Input variable locations for SmartShader
  GLuint _u_mode, _u_model, _u_pv, _u_fade, _u_color;
  GLuint _a_vertex, _a_color;

  /** Taken from the VisuGL library */
  visugl::TrackBall _trackball;
  
  // Parameters
  ColorRGB   _color_bg;
  GLfloat _text_scale;
}; // GL3DCanvas

#endif // GL_3DCANVAS_HPP
