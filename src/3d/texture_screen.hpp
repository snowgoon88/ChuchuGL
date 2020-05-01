/* -*- coding: utf-8 -*- */

#ifndef TEXTURE_SCREEN_HPP
#define TEXTURE_SCREEN_HPP

/** 
 * Test to display a very simple texture.
 */
#include <3d/gl_3dcanvas_screen.hpp>
#include <gl_shader.hpp>
#include <SOIL/SOIL.h>               // Load images

// ***************************************************************************
// ************************************************************* TextureScreen
// ***************************************************************************
class TextureScreen : public GL3DCanvasScreen
{
public:
  TextureScreen( GL3DCanvas& canvas ) : GL3DCanvasScreen( canvas ),
    _model( 1.0f ),
    _mode_param( 0 ), _fade_param( 1.f )
  {
    // Simple VBO with a quad
    std::vector<GLfloat> quad_vtx = {
      // X,Y,Z,        R,G,B, 
      -1, -1, 0,       1, 0, 0,
      1, -1, 0,        0, 1, 0,
      -1, 1, 0,        0, 0, 1,
      1,  1, 0,        1, 1, 1
    };
    //lines
    std::vector<GLuint> quad_idx = {
      0,1, 1,2, 2,0, 2,3, 3,1
    };
    
    _canvas.make_vao( quad_vtx, quad_idx,
                      _quad_faces_vao, _quad_faces_vbo, _quad_idxsides_vbo );

    _canvas.set_param( _mode_param, _fade_param );

    // TextureShader
    _texture_shader = new GLShader( "src/shaders/m_vp_pos_st.v330.glsl",
                                    "src/shaders/texture.f330.glsl" );
    _u_m_tex = _texture_shader->getUniformLocation( "u_model4x4" );
    _u_pv_tex = _texture_shader->getUniformLocation( "u_proj_view4x4" );
    _u_id_tex = _texture_shader->getUniformLocation( "texture_id" );
    _a_pos_tex = _texture_shader->getAttribLocation( "a_pos3" );
    _a_st_tex = _texture_shader->getAttribLocation( "a_tex_pos2" );

    // Textures
    std::vector<GLfloat> texture_vtx = {
      // X,Y,Z,        S,T 
      0.f, -1.5f, 0.f,        0.f, 0.f,
      0.5f, -1.5f, -0.5f,     1.f, 0.f,
      0.f, -1.f, 0.f,         0.f, 1.f,
      0.5f, -1.f, -0.5f,      1.f, 1.f,
    };

    // VAO and VBO for texture
    glGenVertexArrays( 1, &_tex_vao );
    glBindVertexArray( _tex_vao );
    // Build VBO for vertex
    glGenBuffers(1, &_tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _tex_vbo);
    // push xyz+st into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GLfloat ) * texture_vtx.size(),
  		 texture_vtx.data(), GL_STATIC_DRAW);
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
    std::string filename{"ressources/check_texture.png"};
    //std::string filename{"data/stripe_yel_80x80.png"};
    _texture_id = SOIL_load_OGL_texture
      (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Correct Y upside/down
       );
    if(_texture_id == 0)
      std::cerr << "TextureScreen::SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;
    else
      std::cout << "Texture " << filename << " loaded with id=" << _texture_id << std::endl;
    
  }
                                        
  // *********************************************** TextureScreen::destructor
  virtual ~TextureScreen()
  {
    _canvas.delete_vao( _quad_faces_vao, _quad_faces_vbo, _quad_idxsides_vbo );

    glDeleteVertexArrays( 1, &_tex_vao);
    glDeleteBuffers( 1, &_tex_vbo);
    glDeleteTextures( 1, &_texture_id);
    delete _texture_shader;
  }
  // *************************************************** TextureScreen::render
  virtual void render( int win_width, int win_height )
  {
    //std::cout << "__RENDER TextureScreen";
    //std::cout << " " << win_width << " x " << win_height  << std::endl;
    glPushAttrib (GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);
  
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

    // Colored Quad
    _canvas.smart_shader()->use();
    _canvas.set_projection_view_transform( proj_view );
    _canvas.set_model_transform( _model );
     // draw faces
    glBindVertexArray( _quad_faces_vao );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // and lines
    _canvas.set_param( 1, 1.f, false );
    _canvas.set_color_rgb( {1.f, 1.f, 1.f} );
    glDrawElements( GL_LINES, 5*2, GL_UNSIGNED_INT, 0);
    // back to param
    _canvas.set_param( _mode_param, _fade_param );
    glBindVertexArray( 0 );

    //glPopAttrib();

    //glPushAttrib (GL_ENABLE_BIT);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable (GL_DEPTH_TEST);
    //glDepthMask (GL_TRUE);
    
    // Texture Quad
    _texture_shader->use();
    glBindVertexArray( _tex_vao );
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _u_pv_tex, 1, GL_FALSE,
                        glm::value_ptr(proj_view) );
    glUniformMatrix4fv( _u_m_tex, 1, GL_FALSE,
                        glm::value_ptr( _model ));
    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 ); // because GL_TEXTURE1 ) and not _texture_id
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glUseProgram(0);
    glPopAttrib();
  }
    
  // ************************************************ TextureScreen::attributs
protected:
  // VAOs and VBOs
  GLuint _quad_faces_vao, _quad_faces_vbo, _quad_idxsides_vbo;
  GLuint _tex_vao, _tex_vbo, _texture_id;

  GLShader* _texture_shader;
  GLuint _u_m_tex, _u_pv_tex, _u_id_tex;
  GLuint _a_pos_tex, _a_st_tex;

  // Rendering parameters
  glm::mat4 _model;
  GLint     _mode_param;
  GLfloat   _fade_param;
  
protected:
  void create_texshader()
  {
  }
}; // TextureScreen

#endif // TEXTURE_SCREEN_HPP
