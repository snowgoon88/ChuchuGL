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

    // TODO: size vary if window changes, or use viewport
    make_framebuffer( 800, 800 );

    build_quad( true );
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

    glDeleteVertexArrays( 1, &_quad_vao );
    glDeleteBuffers( 1, &_quad_vtx_vbo );
    glDeleteTextures( 1, &_tron_tex_id );
    
    glDeleteFramebuffers( 1, &_distance_fb );
    // TODO: delete textures ?
    
    delete _glow_shader;
    delete _texture_shader;
  }
  // ****************************************************** GlowScreen::render
  virtual void render( int win_width, int win_height )
  {
    // TODO: render colored lines in Framebuffer and display FB in
    // the top-right overlay
    
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
    glPopAttrib();

    // Overlay in top right corner
    glViewport( win_width * 2 / 3, win_height * 2 /3,
                win_width / 3, win_height /3 );
    // view_projection transform
    projection = glm::ortho( -1.f, 1.f, // left;right
                             -1.f, 1.f, // bottom,top
                             1.f, -1.f // near far
                             );    
    glPushAttrib (GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _texture_shader->use();
    glBindVertexArray( _quad_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_tex, 1, GL_FALSE,
                        glm::value_ptr( projection ) );
    // Default model (Identity matrix)
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 ); // because GL_TEXTURE1 ) and not _texture_id
    glBindTexture(GL_TEXTURE_2D, _tron_tex_id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopAttrib();
    
    glUseProgram(0);

  }
protected:
  // *************************************************** GlowScreen::attributs
  // VAOs and VBOs
  GLuint _line_vao, _ls_vao, _ls_col_vao;
  GLuint _ls_vtx_vbo, _line_idx_vbo, _ls_idx_vbo;

  GLShader* _glow_shader;
  GLuint _u_model_glow, _u_pv_glow, _u_radius_glow;
  GLuint _a_vertex_glow, _a_normal_glow, _a_color_glow;

  GLShader* _texture_shader;
  GLuint _u_m_tex, _u_pv_tex, _u_id_tex; 
  GLuint _a_pos_tex, _a_st_tex;
  GLuint _quad_vao;
  GLuint _quad_vtx_vbo;
  GLuint _tron_tex_id;

  GLuint _distance_fb, _distance_tex;  // distance framebuffer and texture
  
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
  void build_quad( bool verb=false )
  {
    if (verb) {
      std::cout << "__CREATE _texture_shader" << std::endl;
    }
    // TextureShader
    _texture_shader = new GLShader( "src/shaders/m_vp_pos_st.v330.glsl",
                                    "src/shaders/texture.f330.glsl" );
    _u_m_tex = _texture_shader->getUniformLocation( "u_model4x4" );
    _u_pv_tex = _texture_shader->getUniformLocation( "u_proj_view4x4" );
    _u_id_tex = _texture_shader->getUniformLocation( "texture_id" );
    _a_pos_tex = _texture_shader->getAttribLocation( "a_pos3" );
    _a_st_tex = _texture_shader->getAttribLocation( "a_tex_pos2" );
    
    std::vector<GLfloat> quad_vtx = {
      // X,Y,Z                S,T
      -1.f, -1.f, 0.f,       0.f, 0.f, 
      1.f, -1.f, 0.f,        1.f, 0.f, 
      -1.f, 1.f, 0.f,        0.f, 1.f, 
      1.f, 1.f, 0.0f,        1.f, 1.f 
    };
    // Build VBO for vertex
    glGenBuffers(1, &_quad_vtx_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _quad_vtx_vbo);
    // push xyz+st into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GLfloat ) * quad_vtx.size(),
  		 quad_vtx.data(), GL_STATIC_DRAW);

    if (verb) {
      std::cout << "  pushed " <<  quad_vtx.size() << " vtx into quad_vtx_vbo" << std::endl;
    }

    // VAO for quad and texture
    glGenVertexArrays( 1, &_quad_vao );
    glBindVertexArray( _quad_vao );
    glBindBuffer(GL_ARRAY_BUFFER, _quad_vtx_vbo );
    // give locations
    glVertexAttribPointer( _a_pos_tex, 3, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(GLfloat), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _a_pos_tex );
    glVertexAttribPointer( _a_st_tex, 2, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(GLfloat), // stride
                           (void*)(3* sizeof(GLfloat)) ); // offset
    glEnableVertexAttribArray( _a_st_tex );
    glBindVertexArray( 0 );

    // load texture
    std::string filename{"ressources/tron.png"};
    _tron_tex_id = SOIL_load_OGL_texture
      (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Correct Y upside/down
       );
    if(_tron_tex_id == 0)
      std::cerr << "TextureScreen::SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;
    else
      std::cout << "Texture " << filename << " loaded with id=" << _tron_tex_id << std::endl;
    
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
  void make_framebuffer( GLsizei width, GLsizei height, bool verb=false )
  {
    // Create FrameBuffer
    glGenFramebuffers( 1, &_distance_fb );
    glBindFramebuffer( GL_FRAMEBUFFER, _distance_fb ); // draw+read

    // Create a texture to act as the ColorBuffer to the Framebuffer
    glGenTextures(1, &_distance_tex);
    glBindTexture( GL_TEXTURE_2D, _distance_tex );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); // no data in the texture
    // TODO: Check Error

    //GLuint color_buffer; // => not needed for ColorBuffer ?
    //glGenRenderbuffers(1, &color_buffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, color_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           _distance_tex, 0);

    // Check Completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
      exit(3);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
  }
}; // GlowScreen


#endif // GLOW_SCREEN_HPP
