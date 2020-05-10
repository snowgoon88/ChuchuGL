/* -*- coding: utf-8 -*- */

#ifndef GLOW_SCREEN_HPP
#define GLOW_SCREEN_HPP

/** 
 * Lessons learned.
 *
 * For a reason still unknown, in `overlay.f330.glsl`, I need to multiply by 2
 * (add to itself) the color given by the sampler from the Tron texture with
 * the coordinates from the distance_surf.
 *
 * blending is USEFULL only for transparency. Here I used it:
 * - MAX of src and current for the distance map
 * - ADD src_alpha and 1-src_alpha for the final rendering.
 *   (maybe MAX, 1, 1 could work)
 *
 * Many nice example here
 * - create Framebuffer as (color_only) rendering surfaces. Could use also 
 *   depth and stencil.
 *   (Maybe linked to "Non-uniform flow control" in texture sampler
 *   @see https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Texture_lookup_in_shader_stages
 * - read pixels value of a buffer => standard output
 * - (SOIL could be used to save screenshots)
 */
#include <3d/gl_3dcanvas_screen.hpp>

class GlowScreen : public GL3DCanvasScreen
{
  using TSurface = struct surface_T {
    GLuint fb;
    GLuint tex;
  };
public:
  // ************************************************* GlowScreen::constructor
  GlowScreen( GL3DCanvas& canvas ) : GL3DCanvasScreen( canvas ),
    _model( 1.0f ),
    _mode_param( 0 ), _fade_param( 1.f ) // vtxcolor, nofade
  {
    make_glow_shader( true );
    make_overlay_shader();
    make_texture_shader( true );
    make_rgba_shader( true );

    build_line( true );
    build_ls_color_vao( true );
    _canvas.set_param( _mode_param, _fade_param ); // vtxcolor, nofade
    build_ls_vao( true );
    build_dummy_distance( true );

    // TODO: size vary if window changes, or use viewport
    _surf_distance = make_framebuffer( 800, 800 );
    _surf_glow = make_framebuffer( 800, 800 );
    _surf_tron = make_framebuffer( 256, 64 );
    _surf_dummy_dist = make_framebuffer( 256, 64 );
    _surf_result = make_framebuffer( 256, 64 );
    // _surf_botright = make_framebuffer( 800, 800 );
    // _surf_topleft = make_framebuffer( 800, 800 );
    

    build_quads( true );

    glClearColor( 0.f, 0.f, 0.f, 1.f );
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
    glDeleteVertexArrays( 1, &_overlay_vao );
    glDeleteVertexArrays( 1, &_dummy_dist_vao );
    glDeleteBuffers( 1, &_dummy_dist_vbo);
    
    glDeleteFramebuffers( 1, &(_surf_distance.fb) );
    glDeleteTextures( 1, &(_surf_distance.tex) );
    glDeleteFramebuffers( 1, &(_surf_glow.fb) );
    glDeleteTextures( 1, &(_surf_glow.tex) );
    glDeleteFramebuffers( 1, &(_surf_tron.fb) );
    glDeleteTextures( 1, &(_surf_tron.tex) );
    glDeleteFramebuffers( 1, &(_surf_dummy_dist.fb) );
    glDeleteTextures( 1, &(_surf_dummy_dist.tex) );    
    glDeleteFramebuffers( 1, &(_surf_result.fb) );
    glDeleteTextures( 1, &(_surf_result.tex) );
    

    delete _rgba_shader;
    delete _glow_shader;
    delete _texture_shader;
    delete _overlay_shader;
  }
  // ****************************************************** GlowScreen::render
  virtual void render( int win_width, int win_height )
  {
    // TODO: render colored lines in Framebuffer and display FB in
    // the top-right overlay
    
    //std::cout << "__RENDER TextureScreen";
    //std::cout << " " << win_width << " x " << win_height  << std::endl;

    // Draw to Distance Framebuffer ********************************************
    glBindFramebuffer( GL_FRAMEBUFFER, _surf_distance.fb );
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    glBlendFunc(GL_ONE, GL_ONE);
    
    // Fill with BackgroundColor
    //_canvas.render_viewport( 0, 0, win_width, win_height );
    glViewport(0, 0, win_width, win_height );
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
    auto view_mod = _canvas.get_trackball_transform() * _model * z_inverter;
    
    // Glow : make distance map on _surf_distance
    _glow_shader->use();
    glUniformMatrix4fv( _u_p_glow, 1, GL_FALSE, glm::value_ptr(projection) );
    glUniformMatrix4fv( _u_vm_glow, 1, GL_FALSE, glm::value_ptr(view_mod) );
    glUniform1f( _u_radius_glow, 0.1f );
    glBindVertexArray( _ls_vao );
    glDrawElements( GL_LINE_STRIP_ADJACENCY, 7, GL_UNSIGNED_INT, 0);

    // And compute Glowing result on another Framebuffer
    glBindFramebuffer( GL_FRAMEBUFFER, _surf_glow.fb );
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, win_width, win_height );

    glDisable(GL_BLEND);
    _overlay_shader->use();
    glBindVertexArray( _overlay_vao );
    glActiveTexture( GL_TEXTURE1 );
    glUniform1f( _u_size_over, 800.f );
    glUniform1i( _u_gradient_over, 1 ); // because GL_TEXTURE1 )
    glBindTexture(GL_TEXTURE_2D, _tron_tex_id );
    glActiveTexture(GL_TEXTURE2);
    glUniform1i( _u_distance_over, 2 );
    glBindTexture(GL_TEXTURE_2D, _surf_distance.tex );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    
    // Main Buffer = final glow effect *****************************************
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, win_width, win_height );
    //glClearColor( 1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    // glBlendEquation(GL_MAX);
    // glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    _overlay_shader->use();
    glBindVertexArray( _overlay_vao );
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_gradient_over, 1 ); // because GL_TEXTURE1 )
    glBindTexture(GL_TEXTURE_2D, _tron_tex_id );
    glActiveTexture(GL_TEXTURE2);
    glUniform1i( _u_distance_over, 2 );
    glBindTexture(GL_TEXTURE_2D, _surf_distance.tex );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // // Corners smallViews will all use the same projection *********************
    auto overlay_proj = glm::ortho( -1.f, 1.f, // left;right
                                    -1.f, 1.f, // bottom,top
                                    1.f, -1.f // near far
                                    );    

    // Draw the Tron texture to _surf_tron *************************************
    glBindFramebuffer( GL_FRAMEBUFFER, _surf_tron.fb );
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glViewport(0, 0, 256, 64 );
    _texture_shader->use();
    glBindVertexArray( _quad_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_tex, 1, GL_FALSE,
                        glm::value_ptr( overlay_proj ) );
    // Default model (Identity matrix)
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 ); // because GL_TEXTURE1 )
    glBindTexture(GL_TEXTURE_2D, _tron_tex_id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Draw the dummy distance texture to _surf_dummy_dist *********************
    glBindFramebuffer( GL_FRAMEBUFFER, _surf_dummy_dist.fb );
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glViewport(0, 0, 256, 64 );
    _rgba_shader->use();
    glBindVertexArray( _dummy_dist_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_rgba, 1, GL_FALSE,
                        glm::value_ptr( overlay_proj ) );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

    // Then Overlay to _surf_result
     glBindFramebuffer( GL_FRAMEBUFFER, _surf_result.fb );
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glViewport(0, 0, 256, 64 );
     _overlay_shader->use();
    glBindVertexArray( _overlay_vao );
    glUniform1f( _u_size_over, 256.f );
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_gradient_over, 1 ); // because GL_TEXTURE1 )
    glBindTexture(GL_TEXTURE_2D, _tron_tex_id );
    glActiveTexture(GL_TEXTURE2);
    glUniform1i( _u_distance_over, 2 );
    glBindTexture(GL_TEXTURE_2D, _surf_dummy_dist.tex );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport(0, 0, win_width, win_height );
    
    // // TopRight Corner = lines *************************************************
    glViewport( win_width * 2 / 3, win_height * 2 /3,
                win_width / 3, win_height /3 );
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_LINE_SMOOTH);
    
    _canvas.smart_shader()->use();
    _canvas.set_projection_view_transform( proj_view );
    _canvas.set_model_transform( _model );
    // glBindVertexArray( _line_vao );
    // glDrawElements( GL_LINES, 2*3, GL_UNSIGNED_INT, 0);
    glBindVertexArray( _ls_col_vao );
    glDrawElements( GL_LINE_STRIP_ADJACENCY, 7, GL_UNSIGNED_INT, 0);


    // BottomRight Corner = distance texture ***********************************
    glViewport( win_width * 2 / 3, 0,
                win_width / 3, win_height /3 );
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    _texture_shader->use();
    glBindVertexArray( _quad_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_tex, 1, GL_FALSE,
                        glm::value_ptr( overlay_proj ) );
    // Default model (Identity matrix)
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 ); // because GL_TEXTURE1 ) and not _texture_id
    glBindTexture(GL_TEXTURE_2D, _surf_distance.tex );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    // Texture in top left *****************************************************
    glViewport( 0, win_height * 4 / 5,
                win_width / 3, win_height / 5 );
    glDisable(GL_DEPTH_TEST);
    glEnable( GL_BLEND );
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // same projection
    _texture_shader->use();
    glBindVertexArray( _quad_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_tex, 1, GL_FALSE,
                        glm::value_ptr( overlay_proj ) );
    // Default model (Identity matrix)
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 ); // because GL_TEXTURE1 )
    glBindTexture(GL_TEXTURE_2D, _tron_tex_id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Results in bottom left **************************************************
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    // same projection
    _texture_shader->use();
    glBindVertexArray( _quad_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_tex, 1, GL_FALSE,
                        glm::value_ptr( overlay_proj ) );
    // Default model (Identity matrix)
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 ); // because GL_TEXTURE1 )

    display_texture( _surf_tron.tex, 0, 0 );
    display_texture( _surf_dummy_dist.tex, 0, 100 );
    display_texture( _surf_result.tex, 0, 200 );

    glUseProgram(0);

  }
  // ********************************************** GlowScreen::draw_functions
  /** 
   * Draw texture at pixel pos
   * Uses _texture_shader => must have set _u_pv_tex before hand
   */
  void display_texture( GLuint tex_id, GLint x, GLint y )
  {
    glViewport( x, y, 256, 64 );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture(GL_TEXTURE_2D, tex_id );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }
  // ************************************************* GlowScreen::read_buffer
  /** Read pixels value from the given buffer */  
  void read_buffer( GLuint buffer_id, const std::string& name )
  {
    GLfloat tron_pixels[256*4*2];
    GLfloat dist_pixels[256*4*2];
    GLfloat resu_pixels[256*4*2];
    
    glBindFramebuffer( GL_FRAMEBUFFER, _surf_tron.fb );
    glReadPixels( 0, 0, 256, 2, GL_RGBA, GL_FLOAT, tron_pixels );

    glBindFramebuffer( GL_FRAMEBUFFER, _surf_dummy_dist.fb );
    glReadPixels( 0, 0, 256, 2, GL_RGBA, GL_FLOAT, dist_pixels );

    glBindFramebuffer( GL_FRAMEBUFFER, _surf_result.fb );
    glReadPixels( 0, 0, 256, 2, GL_RGBA, GL_FLOAT, resu_pixels );    
    
    std::cout << "## " << name << "********************" << std::endl;
    for( unsigned int i = 0; i < 256*2; ++i) {
      std::cout << i << "\t";
      std::cout << tron_pixels[i*4+0] << "\t";
      std::cout << tron_pixels[i*4+1] << "\t";
      std::cout << tron_pixels[i*4+2] << "\t";
      std::cout << tron_pixels[i*4+3] << "\t";

      std::cout << dist_pixels[i*4+0] << "\t";
      std::cout << dist_pixels[i*4+1] << "\t";
      std::cout << dist_pixels[i*4+2] << "\t";
      std::cout << dist_pixels[i*4+3] << "\t";

      std::cout << resu_pixels[i*4+0] << "\t";
      std::cout << resu_pixels[i*4+1] << "\t";
      std::cout << resu_pixels[i*4+2] << "\t";
      std::cout << resu_pixels[i*4+3] << std::endl;      
    }
  }
  // *************************************************** GL3DScreen::callbacks
  virtual void key_callback(int key, int scancode, int action, int mods)
  {
    GL3DCanvasScreen::key_callback(key, scancode, action, mods);
    
    // P
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
      read_buffer( _surf_tron.fb, "tron+dummy_dist+resu" );
    }
  }
protected:
  // *************************************************** GlowScreen::attributs
  // VAOs and VBOs
  GLuint _line_vao, _ls_vao, _ls_col_vao;
  GLuint _ls_vtx_vbo, _line_idx_vbo, _ls_idx_vbo;

  GLShader* _glow_shader;
  GLuint _u_vm_glow, _u_p_glow, _u_radius_glow;
  GLuint _a_vertex_glow, _a_normal_glow, _a_color_glow;

  GLShader* _texture_shader;
  GLuint _u_m_tex, _u_pv_tex, _u_id_tex; 
  GLuint _a_pos_tex, _a_st_tex;
  GLuint _quad_vao;
  GLuint _quad_vtx_vbo;
  GLuint _tron_tex_id;
  GLuint _overlay_vao;
  
  GLShader* _overlay_shader;
  GLuint _u_gradient_over, _u_distance_over, _u_size_over;
  GLuint _a_pos_over;

  GLShader* _rgba_shader;
  GLuint _u_model_rgba, _u_pv_rgba;
  GLuint _a_pos_rgba, _a_col_rgba;
  GLuint  _dummy_dist_vao, _dummy_dist_vbo;
  
  
  TSurface _surf_distance, _surf_glow;
  TSurface _surf_tron, _surf_dummy_dist, _surf_result;
  
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
  /**
   */
  void build_dummy_distance( bool verb=false )
  {
    if (verb) {
      std::cout << "__CREATE dummy_distance VAO" << std::endl;
    }

    std::vector<GLfloat> quad_vtx = {
      // X,Y,Z    R,G,B,A,
      -1.f, 1.f, 0.f,        0.f, 0.f, 0.f, 0.f,
      -1.f, -1.f, 0.f,       0.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f,         1.f, 1.f, 1.f, 1.f,
      0.f, -1.f, 0.f,        1.f, 1.f, 1.f, 1.f,
      1.f, 1.f, 0.f,         0.f, 0.f, 0.f, 0.f,
      1.f, -1.f, 0.f,        0.f, 0.f, 0.f, 0.f
    };
    // Build VBO for vertex
    glGenBuffers(1, &_dummy_dist_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _dummy_dist_vbo);
    // push xyz+rgba into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GLfloat ) * quad_vtx.size(),
  		 quad_vtx.data(), GL_STATIC_DRAW);

    if (verb) {
      std::cout << "  pushed " <<  quad_vtx.size() << " vtx into _dist_dummy_vbo" << std::endl;
    }
    // VAO
    glGenVertexArrays( 1, &_dummy_dist_vao );
    glBindVertexArray( _dummy_dist_vao );
    glBindBuffer(GL_ARRAY_BUFFER, _dummy_dist_vbo);
    // position attribute
    glVertexAttribPointer( _a_pos_rgba, 3, GL_FLOAT, GL_FALSE,
                           (3+4) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _a_pos_rgba );
    // color
    glVertexAttribPointer( _a_col_rgba, 4, GL_FLOAT, GL_FALSE,
                           (3+4) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _a_col_rgba );

    glBindVertexArray( 0 );
  }
  void make_texture_shader( bool verb=false )
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
  }
  void make_overlay_shader( bool verb=false )
  {
     if (verb) {
      std::cout << "__CREATE _overlay_shader" << std::endl;
     }
     // OverlayShader
     _overlay_shader = new GLShader( "src/shaders/pos.v330.glsl",
                                    "src/shaders/overlay.f330.glsl" );
     _u_distance_over = _overlay_shader->getUniformLocation( "u_distance_tex" );
     _u_gradient_over = _overlay_shader->getUniformLocation( "u_gradient_tex" );
     _u_size_over = _overlay_shader->getUniformLocation( "u_size" );
     _a_pos_over = _overlay_shader->getAttribLocation( "a_pos3" );
  }
  void build_quads( bool verb=false )
  {    
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


    // VAO for quad and overlay
    glGenVertexArrays( 1, &_overlay_vao );
    glBindVertexArray( _overlay_vao );
    glBindBuffer(GL_ARRAY_BUFFER, _quad_vtx_vbo );
    // give locations
    glVertexAttribPointer( _a_pos_over, 3, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(GLfloat), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _a_pos_over );
    glBindVertexArray( 0 );

    // load texture
    std::string filename{"ressources/tron.png"};
    //std::string filename{"ressources/check_texture.png"};
    _tron_tex_id = SOIL_load_OGL_texture
      (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Correct Y upside/down
       );
    // This way, interpolation at the frontier will be nice
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    if(_tron_tex_id == 0)
      std::cerr << "TextureScreen::SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;
    else
      std::cout << "Texture " << filename << " loaded with id=" << _tron_tex_id << std::endl;
    
  }
  void make_glow_shader( bool verb=false )
  {
    _glow_shader = new GLShader( "src/shaders/p_vm_rgbnorm.v330.glsl",
                                 "src/shaders/segment_distance.f330.glsl",
                                 "src/shaders/ls_box_growing.g330.glsl"
                                 );
    _u_vm_glow = _glow_shader->getUniformLocation( "u_view_model4x4" );
    _u_p_glow = _glow_shader->getUniformLocation( "u_proj4x4" );
    _u_radius_glow = _glow_shader->getUniformLocation( "u_radius" );
    _a_vertex_glow = _glow_shader->getAttribLocation( "a_pos3" );
    _a_normal_glow = _glow_shader->getAttribLocation( "a_norm3" );
    _a_color_glow = _glow_shader->getAttribLocation( "a_col3" );
  }
  void make_rgba_shader( bool verb = false )
  {
    _rgba_shader = new GLShader("src/shaders/m_vp_pos_rgba.v330.glsl",
                                "src/shaders/rgba.f330.glsl" );
    _u_model_rgba = _rgba_shader->getUniformLocation( "u_model4x4" );
    _u_pv_rgba = _rgba_shader->getUniformLocation( "u_proj_view4x4" );
    _a_pos_rgba = _rgba_shader->getAttribLocation( "a_pos3" );
    _a_col_rgba = _rgba_shader->getAttribLocation( "a_col4" );
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
  TSurface make_framebuffer( GLsizei width, GLsizei height, bool verb=false )
  {
    TSurface surf;
    // Create FrameBuffer
    glGenFramebuffers( 1, &(surf.fb) );
    glBindFramebuffer( GL_FRAMEBUFFER, surf.fb ); // draw+read

    // Create a texture to act as the ColorBuffer to the Framebuffer
    glGenTextures(1, &(surf.tex));
    glBindTexture( GL_TEXTURE_2D, surf.tex );
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
                           surf.tex, 0);

    // Check Completeness
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
      exit(3);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return surf;
  }
}; // GlowScreen


#endif // GLOW_SCREEN_HPP
