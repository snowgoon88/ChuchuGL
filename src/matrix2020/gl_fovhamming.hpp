/* -*- coding: utf-8 -*- */

#ifndef GL_FOCHAMMING_HPP
#define GL_FOCHAMMING_HPP

/** 
 * Display 2 squares
 * // TODO: dynamic VBO
 */

#include <gl_shader.hpp>

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
public:
  // *********************************************** GLFovHamming::constructor
  GLFovHamming()
  {
    _square_shader = new GLShader( "src/shaders/cell.v330.glsl",
                                    "src/shaders/base.f330.glsl",
                                   "src/shaders/square.g330.glsl" );
    _proj_view_loc_square = _square_shader->getUniformLocation( "proj_view" );
    _model_loc_square = _square_shader->getUniformLocation( "model" );
    _c_length_loc_square = _square_shader->getUniformLocation( "c_length" );

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
    float square_vtx[] = {
      0.5f, 0.5f, 0.0f,    1.0f, 1.0f, 0.0f,
        1.5f, 2.5f, 0.0f,    1.0f, 1.0f, 0.0f,
        2.5f, 2.5f, 0.0f,    1.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays( 1, &_square_vao );
    glGenBuffers( 1, &_square_vbo );

    glBindVertexArray( _square_vao );

    glBindBuffer( GL_ARRAY_BUFFER, _square_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(square_vtx), square_vtx,
                  GL_STATIC_DRAW );

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
  // **************************************************** GLFovHamming::render
  void render()
  {
    // create transformations
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);
    glm::mat4 model( 1.0f );
    projection = glm::ortho( -1.f, 10.f, -1.f, 10.0f, 0.1f, 100.0f );
    view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

    // draw squares
    _square_shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc_square, 1, GL_FALSE,
                        glm::value_ptr(projection*view) );
    glUniformMatrix4fv( _model_loc_square, 1, GL_FALSE,
                        glm::value_ptr(model) );

    double square_length = 0.33f;
    glUniform1f( _c_length_loc_square, (float) square_length );
  
    glBindVertexArray(_square_vao);
    glDrawArrays(GL_POINTS, 0, 3 ); // mode, first, count
    
  }
  // ************************************************* GLFovHamming::attributs
  GLShader* _square_shader;
  GLuint _proj_view_loc_square;
  GLuint _model_loc_square;
  GLuint _c_length_loc_square;
  GLuint _square_vao, _square_vbo;
};
// ******************************************************** GLFovHamming - END

}; // namespace matrix2020

#endif // GL_FOCHAMMING_HPP
