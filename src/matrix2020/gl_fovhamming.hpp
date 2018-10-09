/* -*- coding: utf-8 -*- */

#ifndef GL_FOCHAMMING_HPP
#define GL_FOCHAMMING_HPP

/** 
 * Display 2 squares
 * // TODO: dynamic VBO
 */

#include <gl_shader.hpp>

#include <matrix2020/fov_hamming.hpp>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace matrix2020 {
  
// ***************************************************************************
// ************************************************************** GLFovHamming
// ***************************************************************************
class GLFovHamming
{
  using Vertex = struct s_Vertex {
    GLfloat x, y, z; // pos
    GLfloat r, g, b; // color
  };
private:
  // max number of square to draw
  static const unsigned int _max_size = 100;
public:
  // *********************************************** GLFovHamming::constructor
  GLFovHamming(const FOVHamming& fov) : _fov(fov), _square_vtx()
  {
    _square_shader = new GLShader( "src/shaders/cell.v330.glsl",
                                    "src/shaders/base_fade.f330.glsl",
                                   "src/shaders/square.g330.glsl" );
    _proj_view_loc_square = _square_shader->getUniformLocation( "proj_view" );
    _model_loc_square = _square_shader->getUniformLocation( "model" );
    _c_length_loc_square = _square_shader->getUniformLocation( "c_length" );
    _fade_square = _square_shader->getUniformLocation( "fade" );

    build_list_square();
  }
  // *************************************************** GLFovHamming::destroy
  ~GLFovHamming()
  {
    glDeleteVertexArrays( 1, &_square_vao);
    glDeleteBuffers( 1, &_square_vbo);
    delete _square_shader;   
  }
  // ************************************************ GLFovHamming::build_data
  void build_list_square()
  {
    // set up vertex data, each vertex has x,y,z r,g,b
    _square_vtx.clear();
    // _square_vtx.push_back( {0.5f, 0.5f, 0.0f,    1.0f, 1.0f, 0.0f});
    // _square_vtx.push_back( {1.5f, 2.5f, 0.0f,    1.0f, 1.0f, 0.0f});
    // _square_vtx.push_back( {2.5f, 2.5f, 0.0f,    1.0f, 1.0f, 0.0f});
    // _square_vtx.push_back( {2.5f, 4.5f, 0.0f,    1.0f, 1.0f, 0.0f});    
    
    glGenVertexArrays( 1, &_square_vao );
    glGenBuffers( 1, &_square_vbo );
    std::cout << "GLFovHamming array=" << _square_vao << " buff=" << _square_vbo << std::endl;
    
    glBindVertexArray( _square_vao );

    glBindBuffer( GL_ARRAY_BUFFER, _square_vbo );
    // glBufferData( GL_ARRAY_BUFFER, sizeof(square_vtx), square_vtx,
    //               GL_STATIC_DRAW );
    glBufferData( GL_ARRAY_BUFFER,
                  GLFovHamming::_max_size * sizeof(Vertex),
                  NULL, //not _square_vtx.data() as it might be empty
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
  // *********************************************** GLFovHamming::update_data
  void update_data()
  {
    // build list of cell in FOV
    _square_vtx.clear();
    for( auto& cell: _fov._status) {
      auto& pos = cell.first;
      _square_vtx.push_back( {(float)pos.x-0.5f, (float)pos.y-0.5f, 0.f,
            1.0f, 1.0f, 0.0f} );
    }

    // specify the buffer we are about to update
    glBindBuffer( GL_ARRAY_BUFFER, _square_vbo );
    // use std::vector to store data ?
    // ask for reallocation, glBufferData with NULL and same parameters
    // see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming)
    glBufferData( GL_ARRAY_BUFFER,
                  GLFovHamming::_max_size * sizeof(Vertex),
                  NULL, 
                  GL_DYNAMIC_DRAW );
    
    // will call glBufferSubData on the entire buffer
    glBufferSubData( GL_ARRAY_BUFFER,
                     0,                                   //start of sub
                     _square_vtx.size() * sizeof(Vertex), //length sub
                     _square_vtx.data() );                // data
  }
  // **************************************************** GLFovHamming::render
  void render( const glm::mat4& projview )
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // create transformations
    glm::mat4 model( 1.0f );

    // draw squares
    _square_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_square, 1, GL_FALSE,
                        glm::value_ptr( projview ) );
    glUniformMatrix4fv( _model_loc_square, 1, GL_FALSE,
                        glm::value_ptr(model) );

    float square_length = 1.0f;
    glUniform1f( _c_length_loc_square, square_length );
    float square_fade = 0.75f;
    glUniform1f( _fade_square, square_fade );
  
    glBindVertexArray(_square_vao);
    glDrawArrays(GL_POINTS, 0, _square_vtx.size() ); // mode, first, count
    
  }
  // ************************************************* GLFovHamming::attributs
  const FOVHamming& _fov;
  std::vector<Vertex> _square_vtx;
  GLShader* _square_shader;
  GLuint _proj_view_loc_square;
  GLuint _model_loc_square;
  GLuint _c_length_loc_square;
  GLuint _fade_square;
  GLuint _square_vao, _square_vbo;
};
// ******************************************************** GLFovHamming - END

}; // namespace matrix2020

#endif // GL_FOCHAMMING_HPP
