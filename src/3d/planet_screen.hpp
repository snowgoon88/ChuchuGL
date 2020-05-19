/* -*- coding: utf-8 -*- */

#ifndef PLANET_SCREEN_HPP
#define PLANET_SCREEN_HPP

/** 
 * Planet as Icosphere with texture.
 */
#include <3d/gl_3dcanvas_screen.hpp>
#include <gl_shader.hpp>
#include <SOIL/SOIL.h>               // Load images
#include <3d/gl3d_icosphere.hpp>

// ***************************************************************************
// ************************************************************** PlanetScreen
// ***************************************************************************
class PlanetScreen : public GL3DCanvasScreen
{
public:
  PlanetScreen( GL3DCanvas& canvas ) : GL3DCanvasScreen( canvas ),
    _icosphere( true /*verb*/ ),
    _model( 1.0f ),
    _mode_param( 0 ), _fade_param( 1.f )
  {
    make_texture_shader( true /*verb*/ );
    build_vao( true /*verb*/ );
    
    _earthtex_id = load_texture( "data/ico_earth2048x1024.png",
                                 0 /*not inverted*/ );
  }
                                        
  // *********************************************** PlanetScreen::destructor
  virtual ~PlanetScreen()
  {
    delete_vao();
    glDeleteTextures( 1, &_earthtex_id);

    delete_texture_shader( false /*verb*/ );
  }
  // **************************************************** PlanetScreen::render
  virtual void render( int win_width, int win_height )
  {
    //std::cout << "__RENDER PlanetScreen";
    //std::cout << " " << win_width << " x " << win_height  << std::endl;

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);

    // Fill with BackgroundColor
    _canvas.render_viewport( 0, 0, win_width, win_height );
    
    // view_projection transform
    auto projection = glm::ortho( -2.f, 2.f, // left;right
                                  -2.f, 2.f, // bottom,top
                                  100.f, -100.f // near far
                                  );
    // // Invert Z axis as OpenGL is left-handed
    glm::mat4 z_inverter = glm::scale( glm::mat4(1.0f),
                                       glm::vec3( 1, 1, -1 ));
    auto view_model = _canvas.get_trackball_transform()* z_inverter * _model;

    // Textured triangles
    _texture_shader->use();
    glBindVertexArray( _texface_vao );
    glUniformMatrix4fv( _u_p_tex, 1, GL_FALSE, glm::value_ptr(projection) );
    glUniformMatrix4fv( _u_vm_tex, 1, GL_FALSE, glm::value_ptr(view_model) );

    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( _u_id_tex, 1 );
    glBindTexture(GL_TEXTURE_2D, _earthtex_id );

    glDrawArrays( GL_TRIANGLES, 0, 20*3 /*nb vtx*/); // mode, first, count

    // restore GLstate
    glDisable (GL_DEPTH_TEST);
    
  }
    
  // ************************************************* PlanetScreen::attributs
protected:
  // VAOs and VBOs
  GL3DIcosphere _icosphere;
  GLuint _texface_vao;
  GLuint _earthtex_id;
  
  // Rendering parameters
  glm::mat4 _model;
  GLint     _mode_param;
  GLfloat   _fade_param;

  // Shaders
  GLShader* _texture_shader;
  GLuint _u_vm_tex, _u_p_tex, _u_id_tex; 
  GLuint _a_pos_tex, _a_st_tex, _a_nor_tex;
  
  // *************************************************** PlanetScreen::shaders  
protected:
  void make_texture_shader( bool verb=false )
  {
    if (verb) {
      std::cout << "__SHADER create _texture_shader" << std::endl;
    }
     _texture_shader = new GLShader( "src/shaders/p_vm_stnorm.v330.glsl",
                                    "src/shaders/tex_nor.f330.glsl" );
    _u_vm_tex = _texture_shader->getUniformLocation( "u_view_model4x4" );
    _u_p_tex = _texture_shader->getUniformLocation( "u_proj4x4" );
    _u_id_tex = _texture_shader->getUniformLocation( "texture_id" );
    _a_pos_tex = _texture_shader->getAttribLocation( "a_pos3" );
    _a_st_tex = _texture_shader->getAttribLocation( "a_postex2" );
    _a_nor_tex = _texture_shader->getAttribLocation( "a_norm3" );
  }
  void delete_texture_shader( bool verb=false )
  {
    if (verb) {
      std::cout << "__SHADER delete _texture_shader" << std::endl;
    }
    delete _texture_shader;
  }
  // ********************************************** PlanetScreen::load_texture
  unsigned int load_texture( const std::string& pathname,
                             unsigned int flags=SOIL_FLAG_INVERT_Y )
  {
    
    unsigned int tex_id = SOIL_load_OGL_texture
      (
       pathname.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       flags                                     // Correct Y upside/down ?
       );
    if( tex_id == 0)
      std::cerr << "PlanetScreen::SOIL loading error: '" << SOIL_last_result() << "' (" << pathname << ")" << std::endl;
    else
      std::cout << "Texture " << pathname << " loaded with id=" << tex_id << std::endl;

    return tex_id;
  }
  // ******************************************************* PlanetScreen::VAO
  void build_vao( bool verb=false )
  {
    // VAO with vtx and texture coordinates
    glGenVertexArrays( 1, &_texface_vao );
    glBindVertexArray( _texface_vao );

    // VBO with vertex xyz + st + normals
    glBindBuffer( GL_ARRAY_BUFFER, _icosphere.icosahedron_face_vbo() );
    // location of vtx
    glVertexAttribPointer( _a_pos_tex, 3, GL_FLOAT, GL_FALSE,
                           (3+2+3) * sizeof(float), // stride
                           (void*)(0 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _a_pos_tex);
    // location of st
    glVertexAttribPointer( _a_st_tex, 2, GL_FLOAT, GL_FALSE,
                           (3+2+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _a_st_tex );
    
    glBindVertexArray( 0 );

    if (verb) {
      std::cout << "__MAKE_VAO _texface_vao=" << _texface_vao;
    }
  }
  void delete_vao()
  {
    glDeleteVertexArrays( 1, &_texface_vao );
  }
}; // PlanetScreen

#endif // PLANET_SCREEN_HPP
