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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define HALF_SIZE (0.5f)

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
    _proj_view_loc_tex = _texture_shader->getUniformLocation( "proj_view" );
    _texture_loc_tex = _texture_shader->getUniformLocation( "texture_id" );

    _cell_shader = new GLShader( "src/shaders/cell.v330.glsl",
                                "src/shaders/base.f330.glsl",
                                "src/shaders/cell.g330.glsl" );
    _proj_view_loc_cell = _cell_shader->getUniformLocation( "proj_view" );
    _model_loc_cell = _cell_shader->getUniformLocation( "model" );
    _c_length_loc_cell = _cell_shader->getUniformLocation( "c_length" );
    
    build_wall_data();
    build_cell_data();
  }
  // ************************************************** GLEnvironment::destroy
  ~GLEnvironment()
  {
    glDeleteVertexArrays( 1, &_wall_vao);
    glDeleteBuffers( 1, &_wall_vbo);
    glDeleteTextures( 1, &_texture_id);
    delete _texture_shader;

    glDeleteVertexArrays( 1, &_cell_vao);
    glDeleteBuffers( 1, &_cell_vbo);
    delete _cell_shader;    
  }
  // ****************************************** GLEnvironment::build_wall_data
  /**
   * Use the model to build a VBA with
   * - VBO : vertex x xyzst (xyz:pos vertex, st:coord texture)
   */
  void build_wall_data()
  {
    // seek position of the 'X' in the Environment
    auto wall_list = _env.get_wall_list();
    
    // set up vertex data, each vertex has x,y,z,s,t 
    float wall_vtx[wall_list.size() * 6*(3+2)];
    unsigned int idx = 0;
    for( auto& wall: wall_list) {
      // First triangle. bottom left
      // x,y,z
      bottom_left_corner(wall_vtx, idx, wall);
      // s,t
      wall_vtx[idx++] = (float) 0.0f;
      wall_vtx[idx++] = (float) 0.0f;
      // First triangle. bottom right
      // x,y,z
      bottom_right_corner(wall_vtx, idx, wall);
      // s,t
      wall_vtx[idx++] = (float) 1.0f;
      wall_vtx[idx++] = (float) 0.0f;
      // First triangle. top right
      // x,y,z
      top_right_corner(wall_vtx, idx, wall);
      // s,t
      wall_vtx[idx++] = (float) 1.0f;
      wall_vtx[idx++] = (float) 1.0f;
      // Second triangle, top right
      // x,y,z
      top_right_corner( wall_vtx, idx, wall);
      // s,t
      wall_vtx[idx++] = (float) 1.0f;
      wall_vtx[idx++] = (float) 1.0f;
      // Second triangle, top left
      // x,y,z
      top_left_corner( wall_vtx, idx, wall );
      // s,t
      wall_vtx[idx++] = (float) 0.0f;
      wall_vtx[idx++] = (float) 1.0f;
      // Second triangle, bottom left
      // x,y,z
      bottom_left_corner( wall_vtx, idx, wall );
      // s,t
      wall_vtx[idx++] = (float) 0.0f;
      wall_vtx[idx++] = (float) 0.0f;
    }

    glGenVertexArrays( 1, &_wall_vao );
    glGenBuffers( 1, &_wall_vbo );

    glBindVertexArray( _wall_vao );

    glBindBuffer( GL_ARRAY_BUFFER, _wall_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(wall_vtx), wall_vtx, GL_STATIC_DRAW );

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
    std::string filename{"data/stripe_yel_80x80.png"};
    std::cout << "__SOIL with " << filename  << std::endl;
    //glActiveTexture(GL_TEXTURE0);
    std::cout << "  active texture" << std::endl;
    _texture_id = SOIL_load_OGL_texture
      (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Corrige les Y upside/down
       );
    std::cout << "  soil finished" << std::endl;
    if(_texture_id == 0)
      std::cerr << "GLSprite: SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;

    // tell OpenGl for each sampler to which texture it belongs to
    // XXX.use();
    // glUniformli( glGetUniformLocation( ourShader.ID, "texture1"), 0);
    // ou
    //   ourShader(.setInt( "texture1", 0 );
                                         
  }
  void build_cell_data()
  {
    // seek position of the '.' in the Environment
    auto cell_list = _env.get_cell_list();
    
    // set up vertex data, each vertex has x,y,z, r,g,b 
    float cell_vtx[cell_list.size() * 2*(3+3)];
    unsigned int idx = 0;
    for( auto& cell: cell_list) {
      // bottom left
      bottom_left_corner( cell_vtx, idx, cell );
      // r,g,b
      cell_vtx[idx++] = (float) 1.0f;
      cell_vtx[idx++] = (float) 219.0 / 255.0f;
      cell_vtx[idx++] = (float) 0.0f;

      // // Then bottom right
      // bottom_right_corner( cell_vtx, idx, cell );
      // // r,g,b
      // cell_vtx[idx++] = (float) 0.0f;
      // cell_vtx[idx++] = (float) 1.0f;
      // cell_vtx[idx++] = (float) 1.0f;
    }

    glGenVertexArrays( 1, &_cell_vao );
    glGenBuffers( 1, &_cell_vbo );

    glBindVertexArray( _cell_vao );

    glBindBuffer( GL_ARRAY_BUFFER, _cell_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(cell_vtx), cell_vtx, GL_STATIC_DRAW );

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
  // *************************************************** GLEnvironment::render
  void render()
  {
    // create transformations
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);
    glm::mat4 model( 1.0f );
    projection = glm::ortho( -1.f, 10.f, -1.f, 10.0f, 0.1f, 100.0f );
    view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    
    // activate program
    _texture_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_tex, 1, GL_FALSE,
                        glm::value_ptr(projection*view) );
    // tell what texture => TEXTURE0
    // TODO could be done once, before
    // glUniform1i( _texture_loc_tex, 0 ); //car GL_TEXTURE0 ) and not _texture_id );

    // bind textures on corresponding texture units
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture_id);

    // draw
    glBindVertexArray( _wall_vao );
    glDrawArrays( GL_TRIANGLES, 0, _env.get_wall_list().size() * 6); // mode, first, count

    // and now the cells
    _cell_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_cell, 1, GL_FALSE,
                        glm::value_ptr(projection*view) );
    glUniformMatrix4fv( _model_loc_cell, 1, GL_FALSE,
                        glm::value_ptr(model) );

    //double cell_length = sin( glfwGetTime()*5.0 ) * 0.1 + 0.25;
    double cell_length = 0.15f;
    glUniform1f( _c_length_loc_cell, (float) cell_length );
  
    glBindVertexArray(_cell_vao);
    glDrawArrays(GL_POINTS, 0, _env.get_cell_list().size() * 1 ); // mode, first, count
    
  }
  // ************************************************ GLEnvironment::attributs
  const Environment& _env;
  GLShader* _texture_shader;
  GLuint _texture_loc_tex;
  GLuint _proj_view_loc_tex;
  GLuint _texture_id;
  GLuint _wall_vao, _wall_vbo;

  GLShader* _cell_shader;
  GLuint _proj_view_loc_cell;
  GLuint _model_loc_cell;
  GLuint _c_length_loc_cell;
  GLuint _cell_vao, _cell_vbo;

private:
  // ************************************************** GLEnvironment::corners
  void bottom_left_corner( float *vtx, unsigned int& idx, const Pos& pos )
  {
    vtx[idx++] = (float) pos.x - HALF_SIZE;
    vtx[idx++] = (float) pos.y - HALF_SIZE;
    vtx[idx++] = 0.f;
  }
  void bottom_right_corner( float *vtx, unsigned int& idx, const Pos& pos )
  {
    vtx[idx++] = (float) pos.x + HALF_SIZE;
    vtx[idx++] = (float) pos.y - HALF_SIZE;
    vtx[idx++] = 0.f;
  }
  void top_left_corner( float *vtx, unsigned int& idx, const Pos& pos )
  {
    vtx[idx++] = (float) pos.x - HALF_SIZE;
    vtx[idx++] = (float) pos.y + HALF_SIZE;
    vtx[idx++] = 0.f;
  }
  void top_right_corner( float *vtx, unsigned int& idx, const Pos& pos )
  {
    vtx[idx++] = (float) pos.x + HALF_SIZE;
    vtx[idx++] = (float) pos.y + HALF_SIZE;
    vtx[idx++] = 0.f;
  }
}; // class GLEnvironment

}; // namespace matrix2020


#endif // GL_ENVIRONMENT_HPP
