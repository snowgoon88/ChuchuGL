/* -*- coding: utf-8 -*- */

#ifndef GL_ENVIRONMENT_HPP
#define GL_ENVIRONMENT_HPP

/** 
 * OpenGL to render Environment.
 * - 'X' as texture
 */

#include <matrix2020/m_environment.hpp>

#include <gl_shader.hpp>
#include <SOIL/SOIL.h>               // Load images
#include <stb_image.h>               // alt to load images

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace matrix2020 {
// ***************************************************************************
// ************************************************************* GLEnvironment
// ***************************************************************************
class GLEnvironment
{
public:
  // ************************************************* GLEnvironment::creation
  GLEnvironment( const Environment& env ) :
    _env(env)
  {
    _texture_shader = new GLShader( "src/shaders/texture.v330.glsl",
                                    "src/shaders/texture.f330.glsl" );
    _proj_view_loc = _texture_shader->getUniformLocation( "proj_view" );
    _texture_loc = _texture_shader->getUniformLocation( "texture_id" );

    build_vba();
  }
  // ************************************************** GLEnvironment::destroy
  ~GLEnvironment()
  {
    glDeleteVertexArrays( 1, &_vao);
    glDeleteBuffers( 1, &_vbo);
    glDeleteTextures( 1, &_texture_id);
    delete _texture_shader;
  }
  // ************************************************ GLEnvironment::build_vba
  /**
   * Use the model to build a VBA with
   * - VBO : vertex x xyzst (xyz:pos vertex, st:coord texture)
   */
  void build_vba()
  {
    // seek position of the 'X' in the Environment
    auto wall_list = _env.get_wall_list();
    
    // set up vertex data, each vertex has x,y,z,s,t 
    float vertices[wall_list.size() * 6*(3+2)];
    unsigned int idx = 0;
    for( auto& wall: wall_list) {
      // First triangle. bottom left
      // x,y,z
      vertices[idx++] = (float) wall.x - 0.5f;
      vertices[idx++] = (float) wall.y - 0.5f;
      vertices[idx++] = 0.f;
      // s,t
      vertices[idx++] = (float) 0.0f;
      vertices[idx++] = (float) 0.0f;
      // First triangle. bottom right
      // x,y,z
      vertices[idx++] = (float) wall.x + 0.5f;
      vertices[idx++] = (float) wall.y - 0.5f;
      vertices[idx++] = 0.f;
      // s,t
      vertices[idx++] = (float) 1.0f;
      vertices[idx++] = (float) 0.0f;
      // First triangle. top right
      // x,y,z
      vertices[idx++] = (float) wall.x + 0.5f;
      vertices[idx++] = (float) wall.y + 0.5f;
      vertices[idx++] = 0.f;
      // s,t
      vertices[idx++] = (float) 1.0f;
      vertices[idx++] = (float) 1.0f;
      // Second triangle, top right
      // x,y,z
      vertices[idx++] = (float) wall.x + 0.5f;
      vertices[idx++] = (float) wall.y + 0.5f;
      vertices[idx++] = 0.f;
      // s,t
      vertices[idx++] = (float) 1.0f;
      vertices[idx++] = (float) 1.0f;
      // Second triangle, top left
      // x,y,z
      vertices[idx++] = (float) wall.x - 0.5f;
      vertices[idx++] = (float) wall.y + 0.5f;
      vertices[idx++] = 0.f;
      // s,t
      vertices[idx++] = (float) 0.0f;
      vertices[idx++] = (float) 1.0f;
      // Second triangle, bottom left
      // x,y,z
      vertices[idx++] = (float) wall.x - 0.5f;
      vertices[idx++] = (float) wall.y - 0.5f;
      vertices[idx++] = 0.f;
      // s,t
      vertices[idx++] = (float) 0.0f;
      vertices[idx++] = (float) 0.0f;
    }

    glGenVertexArrays( 1, &_vao );
    glGenBuffers( 1, &_vbo );

    glBindVertexArray( _vao );

    glBindBuffer( GL_ARRAY_BUFFER, _vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    // position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray(0);
    // texture coordinate
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE,
                           (3+2) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray(1);
    // Delier les VBO ??
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray( 0 );
    
    // load and create texture
    // //std::string filename{"data/stripe_yel_80x80.png"};
    // //std::string filename{"/home/alain/Projets/ChuchuGL/data/stripe_yel_80x80.png"};
    // std::string filename{"/home/alain/Projets/ChuchuGL/Images/tex_titlerocket.png"};
    // std::cout << "__SOIL with " << filename  << std::endl;
    // //glActiveTexture(GL_TEXTURE0);
    // std::cout << "  active texture" << std::endl;
    // _texture_id = SOIL_load_OGL_texture
    //   (
    //    filename.c_str(),                         // pathfile
    //    //"Images/tex_titlerocket.png",
    //    //"/home/alain/Projets/ChuchuGL/data/stripe_yel_80x80.png",
    //    SOIL_LOAD_AUTO,                           // 
    //    SOIL_CREATE_NEW_ID,                       //
    //    SOIL_FLAG_INVERT_Y                        // Corrige les Y upside/down
    //    );
    // std::cout << "  soil finished" << std::endl;
    // if(_texture_id == 0)
    //   std::cerr << "GLSprite: SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;

    glGenTexture( 1, &_texture_id);
    glBindTexture( GL_TEXTURE_2D, _texture_id );
    // set wrapping parameters (repeat, usual default)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // set filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTERING, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTERING, GL_LINEAR );
    // load image, create texture and generate mipmap
    int img_w, img_h, img_nb_channels;
    stbi_set_flip_vertically_on_load( true ); // flip image on y-axis
    unsigned char *img_data = stbi_load( filename.c_str(),
                                         &img_w, &img_h, &img_nb_channels, 0 );
    if (img_data) {
      glTexImage2D( GL_TEXTURE_2D, 0,
                    GL_RGB, img_w, img_h, 0,
                    GL_RGB, GL_UNSIGNED_BYTE, img_data );
      glGenerateMipmap( GL_TEXTURE_2D );
    }
    else {
      std::cerr << "GLSprite: STB loading error:  (" << filename << ")" << std::endl;
    }

    stbi_image_free(img_data);

    // tell OpenGl for each sampler to which texture it belongs to
    XXX.use();
    glUniformli( glGetUniformLocation( ourShader.ID, "texture1"), 0);
    ou
      ourShader(.setInt( "texture1", 0 );
                                         
  }
  // *************************************************** GLEnvironment::render
  void render()
  {
    // create transformations
    glm::mat4 view;
    glm::mat4 projection;
    projection = glm::ortho( -10.f, 10.f, -10.f, 10.0f, 0.1f, 100.0f );
    view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    
    // activate program
    _texture_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc, 1, GL_FALSE,
                        glm::value_ptr(projection*view) );
    // tell what texture => TEXTURE0
    // TODO could be done once, before
    glUniform1i( _texture_loc, 0);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    // draw
    glBindVertexArray( _vao );
    glDrawArrays( GL_TRIANGLES, 0, _env.get_wall_list().size()); // mode, first, count
  }
  // ************************************************ GLEnvironment::attributs
  const Environment& _env;
  GLShader* _texture_shader;
  GLuint _texture_loc;
  GLuint _proj_view_loc;
  GLuint _texture_id;
  GLuint _vao, _vbo;
  
}; // class GLEnvironment

}; // namespace matrix2020


#endif // GL_ENVIRONMENT_HPP
