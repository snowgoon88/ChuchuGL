/* -*- coding: utf-8 -*- */

#ifndef GL_CURSOR_HPP
#define GL_CURSOR_HPP

/** 
 * OpenGL to render a cursor around some cells.
 * TODO: possibly with info on the side
 */

#include <gl_shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace matrix2020 {
// ***************************************************************************
// ****************************************************************** GLCursor
// ***************************************************************************
class GLCursor
{
  using Vertex = struct s_Vertex {
    GLfloat x, y, z; // pos
  };
private:
  // max number of agent to draw
  static const unsigned int _max_size = 10;
public:
  // ****************************************************** GLCursor::creation
  GLCursor( const Pos& cursor ) : _cursor(cursor)
  {
    _shader = new GLShader( "src/shaders/pos_mvp.v330.glsl",
                            "src/shaders/unicol.f330.glsl" );
    _proj_view_loc = _shader->getUniformLocation( "proj_view" );
    _model_loc = _shader->getUniformLocation( "model" );
    _color_loc = _shader->getUniformLocation( "color_in" );

    float vertices[] = {
      // x,y,z
      -0.5f, -0.5f, 0.1f,
      0.5f, -0.5f, 0.1f,
      0.5f, 0.5f, 0.1f,
      -0.5f, 0.5f, 0.1f,
      -0.5f, -0.5f, 0.1f
    };
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    std::cout << "GLCursor array=" << _vao << " buff=" << _vbo << std::endl;
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);
  
    glBindBuffer( GL_ARRAY_BUFFER, _vbo);
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    // position attribute of shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), // stride
                          (void*)0);         // offset
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
  }
  // ******************************************************* GLCursor::destroy
  virtual ~GLCursor()
  {
    glDeleteVertexArrays( 1, &_vao);
    glDeleteBuffers( 1, &_vbo);
    delete _shader;
  }
  // ******************************************************** GLCursor::render
  void render( const glm::mat4& projview )
  {
    // create transformations
    glm::mat4 model( 1.0f );
    
    model      = glm::translate(model, glm::vec3( _cursor.x, _cursor.y, 0.f));
    
    // activate program
    _shader->use();
    glBindVertexArray( _vao );

    // color
    glUniform3f( _color_loc, 1.0f, 0.0f, 0.0f );
    
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc, 1, GL_FALSE,
                        glm::value_ptr( projview ));
    glUniformMatrix4fv( _model_loc, 1, GL_FALSE,
                        glm::value_ptr(model) );

    // draw cursor
    glDrawArrays(GL_LINE_STRIP, 0, 5); // mode, first, count
  }
  // ***************************************************** GLCursor::attribut
  const Pos& _cursor; 
  GLShader* _shader;
  GLuint _proj_view_loc, _model_loc;
  GLuint _color_loc;
  GLuint _vao, _vbo;
  
}; // GLCursor
}; // namespace matrix2020

#endif // GL_CURSOR_HPP
