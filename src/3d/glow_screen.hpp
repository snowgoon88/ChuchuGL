/* -*- coding: utf-8 -*- */

#ifndef GLOW_SCREEN_HPP
#define GLOW_SCREEN_HPP

/** 
 * TODO
 */
#include <3d/gl_3dcanvas_screen.hpp>

class GlowScreen : public GL3DCanvasScreen
{
public:
  // ************************************************* GlowScreen::constructor
  GlowScreen( GL3DCanvas& canvas ) : GL3DCanvasScreen( canvas ),
    _model( 1.0f ),
    _mode_param( 0 ), _fade_param( 1.f ) // vtxcolor, nofade
  {
    build_line( true );
    build_ls_color_vao( true );
    _canvas.set_param( _mode_param, _fade_param ); // vtxcolor, nofade
    make_glow_shader( true );
    build_ls_vao( true );
  }
  // ************************************************** GlowScreen::destructor
  virtual ~GlowScreen()
  {
    glDeleteVertexArrays( 1, &_line_vao);
    glDeleteVertexArrays( 1, &_ls_vao);
    glDeleteVertexArrays( 1, &_ls_col_vao);
    glDeleteBuffers( 1, &_ls_vtx_vbo);
    glDeleteBuffers( 1, &_line_idx_vbo);
    glDeleteBuffers( 1, &_ls_idx_vbo);

    delete _glow_shader;
  }
  // ****************************************************** GlowScreen::render
  virtual void render( int win_width, int win_height )
  {
    //std::cout << "__RENDER TextureScreen";
    //std::cout << " " << win_width << " x " << win_height  << std::endl;
    
  
    // Fill with BackgroundColor
    _canvas.render_viewport( 0, 0, win_width, win_height );
    
    // view_projection transform
    auto projection = glm::ortho( -2.f, 2.f, // left;right
                                  -2.f, 2.f, // bottom,top
                                  100.f, -100.f // near far
                                  );
    auto vp_track = projection * _canvas.get_trackball_transform();
    // // Invert Z axis as OpenGL is left-handed
    glm::mat4 z_inverter = glm::scale( glm::mat4(1.0f),
                                       glm::vec3( 1, 1, -1 ));
    auto proj_view = vp_track * z_inverter;

    // Glow
    glPushAttrib (GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);
    _glow_shader->use();
    glUniformMatrix4fv( _u_pv_glow, 1, GL_FALSE, glm::value_ptr(proj_view) );
    glUniformMatrix4fv( _u_model_glow, 1, GL_FALSE, glm::value_ptr(_model) );
    glUniform1f( _u_radius_glow, 0.1f );
    glBindVertexArray( _ls_vao );
    glDrawElements( GL_LINE_STRIP_ADJACENCY, 7, GL_UNSIGNED_INT, 0);
    glBindVertexArray( 0 );
    glPopAttrib();
 
    glPushAttrib (GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable (GL_DEPTH_TEST);
    //glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);
    // Draw lines
    _canvas.smart_shader()->use();
    _canvas.set_projection_view_transform( proj_view );
    _canvas.set_model_transform( _model );
    // glBindVertexArray( _line_vao );
    // glDrawElements( GL_LINES, 2*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray( _ls_col_vao );
    glDrawElements( GL_LINE_STRIP_ADJACENCY, 7, GL_UNSIGNED_INT, 0);
    glBindVertexArray( 0 );

    glUseProgram(0);
    glPopAttrib();
  }
protected:
  // *************************************************** GlowScreen::attributs
  // VAOs and VBOs
  GLuint _line_vao, _ls_vao, _ls_col_vao;
  GLuint _ls_vtx_vbo, _line_idx_vbo, _ls_idx_vbo;

  GLShader* _glow_shader;
  GLuint _u_model_glow, _u_pv_glow, _u_radius_glow;
  GLuint _a_vertex_glow, _a_normal_glow, _a_color_glow;
  
  // Rendering parameters
  glm::mat4 _model;
  GLint     _mode_param;
  GLfloat   _fade_param;

  // ******************************************************* GlowScreen::build
  void build_line( bool verb=false )
  {
    // Line with color and normals
    // End points are repeated twice, as line_stripes for glow
    std::vector<GLfloat> line_vtx = {
      // X,Y,Z               R,G,B     normal
      -0.2f, 0.9f, 0.f,      0,1,0,    0.f,0.f,1.f,

      0.8f, 0.5f, 0.f,       0,0,1,    0.f,0.f,1.f,

      0.3f, -0.3f, 0.f,      0,1,1,    0.f,0.f,1.f,

      -0.3f, 0.f, 0.f,       1,0,1,    0.f,0.f,1.f,

    };
    //classic lines 
    std::vector<GLuint> line_idx = {
      0,1, 1,2, 2,3
    };
    //line_stripes_adjacency
    // first and last points will not be drawn.
    std::vector<GLuint> ls_idx = {
      3, 0, 1, 2, 3, 0, 1
    };
   
    // Build VBO for vertex
    glGenBuffers(1, &_ls_vtx_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _ls_vtx_vbo);
    // push xyz+rgb into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GLfloat ) * line_vtx.size(),
  		 line_vtx.data(), GL_STATIC_DRAW);

    if (verb) {
      std::cout << "  pushed " <<  line_vtx.size() << " vtx into ls_vtx_vbo" << std::endl;
    }
    // Build VBO for line indices
    glGenBuffers(1, &_line_idx_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _line_idx_vbo );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * line_idx.size(),
  		 line_idx.data(), GL_STATIC_DRAW);
    
    if (verb) {
      std::cout << "  pushed " << line_idx.size() << " idx into line_idx_vbo" << std::endl;
    }
    // Build VBO for line_stripe indices
    glGenBuffers(1, &_ls_idx_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ls_idx_vbo );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * ls_idx.size(),
  		 ls_idx.data(), GL_STATIC_DRAW);
    
    if (verb) {
      std::cout << "  pushed " << ls_idx.size() << " idx into ls_idx_vbo" << std::endl;
    }
    

    std::cout << "__MAKE_VAO vao=" << _ls_vao;
    std::cout << " vtx_vbo=" << _ls_vtx_vbo;
    std::cout << " line_idx=" << _line_idx_vbo << std::endl;
    std::cout << " ls_idx=" << _ls_idx_vbo << std::endl;

    // VAO for faces and lines
    glGenVertexArrays( 1, &_line_vao );
    glBindVertexArray( _line_vao );
    glBindBuffer(GL_ARRAY_BUFFER, _ls_vtx_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _line_idx_vbo );
    // position attribute
    glVertexAttribPointer( _canvas.vertex_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _canvas.vertex_attribut() );
    // color
    glVertexAttribPointer( _canvas.color_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _canvas.color_attribut() );
    
    // Unbind VAO
    // TODO: test without unbinding...
    glBindVertexArray( 0 );
  }
  void make_glow_shader( bool verb=false )
  {
    _glow_shader = new GLShader( "src/shaders/m_vp_norm.v330.glsl",
                                 "src/shaders/segment_distance.f330.glsl",
                                 "src/shaders/ls_box_growing.g330.glsl"
                                 );
    _u_model_glow = _glow_shader->getUniformLocation( "u_model4x4" );
    _u_pv_glow = _glow_shader->getUniformLocation( "u_proj_view4x4" );
    _u_radius_glow = _glow_shader->getUniformLocation( "u_radius" );
    _a_vertex_glow = _glow_shader->getAttribLocation( "a_pos3" );
    _a_normal_glow = _glow_shader->getAttribLocation( "a_norm3" );
    _a_color_glow = _glow_shader->getAttribLocation( "a_col3" );
  }
  void build_ls_vao( bool verb=false )
  {
    // VAO for faces and lines
    glGenVertexArrays( 1, &_ls_vao );
    glBindVertexArray( _ls_vao );
    glBindBuffer(GL_ARRAY_BUFFER, _ls_vtx_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ls_idx_vbo );
    // position attribute
    glVertexAttribPointer( _a_vertex_glow, 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _a_vertex_glow );
    // color
    glVertexAttribPointer( _a_color_glow, 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _a_color_glow );
    glVertexAttribPointer( _a_normal_glow, 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)(6 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _a_normal_glow );
    
    // Unbind VAO
    // TODO: test without unbinding...
    glBindVertexArray( 0 );
  }
  void build_ls_color_vao( bool verb=false )
  {
    // VAO for faces and lines
    glGenVertexArrays( 1, &_ls_col_vao );
    glBindVertexArray( _ls_col_vao );
    glBindBuffer(GL_ARRAY_BUFFER, _ls_vtx_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ls_idx_vbo );
    // position attribute
    glVertexAttribPointer( _canvas.vertex_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _canvas.vertex_attribut() );
    // color
    glVertexAttribPointer( _canvas.color_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _canvas.color_attribut() );
    
    // Unbind VAO
    // TODO: test without unbinding...
    glBindVertexArray( 0 );
  }
  
}; // GlowScreen


#endif // GLOW_SCREEN_HPP
