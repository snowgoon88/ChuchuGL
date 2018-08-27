/* -*- coding: utf-8 -*- */

#ifndef GL_HACKER_HPP
#define GL_HACKER_HPP

/** 
 * OpenGL to render Hacker
 */

#include <matrix2020/m_gameobject.hpp>

#include <gl_shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define HACKER_LENGTH (0.25f)

namespace matrix2020 {

// ***************************************************************************
// ****************************************************************** GLHacker
// ***************************************************************************
class GLHacker
{
public:
  // ****************************************************** GLHacker::creation
  GLHacker( GameObjectPtr hacker ) :
    _hacker(hacker)
  {
    _shader = new GLShader( "src/shaders/pos_geom_mvp.v330.glsl",
                            "src/shaders/unicol.f330.glsl",
                            "src/shaders/hacker.g330.glsl" );
    _proj_view_loc = _shader->getUniformLocation( "proj_view" );
    _model_loc = _shader->getUniformLocation( "model" );
    _length_loc = _shader->getUniformLocation( "length" );
    _color_loc = _shader->getUniformLocation( "color_in" );
    _pos_loc = _shader->getUniformLocation( "pos_uni" );

    float vertices[] = {
      // dummy x,y,z
      0.0f, 0.0f, 0.0f
    };
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(_vao);
  
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);

    // position attribute of shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
  }
  // ******************************************************* GLHacker::destroy
  virtual ~GLHacker()
  {
    glDeleteVertexArrays( 1, &_vao);
    glDeleteBuffers( 1, &_vbo);
    delete _shader;
  }
  // ******************************************************** GLHacker::render
  void render()
  {
    // create transformations
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);
    glm::mat4 model( 1.0f );
    projection = glm::ortho( -1.f, 10.f, -1.f, 10.0f, 0.1f, 100.0f );
    view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    
    // activate program
    _shader->use();
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc, 1, GL_FALSE,
                        glm::value_ptr(projection*view) );
    glUniformMatrix4fv( _model_loc, 1, GL_FALSE,
                        glm::value_ptr(model) );

    // length and color
    glUniform1f( _length_loc, HACKER_LENGTH );
    glUniform3f( _color_loc, 0.0f, 1.0f, 1.0f );

    // position
    glUniform3f( _pos_loc,
                 (float) _hacker->pos().x, (float) _hacker->pos().y,
                 0.0f );

    glBindVertexArray( _vao );
    glDrawArrays(GL_POINTS, 0, 1); // mode, first, count
    
    
  }
  // ***************************************************** GLHacker::attribut
  GameObjectPtr _hacker;
  GLShader* _shader;
  GLuint _proj_view_loc, _model_loc;
  GLuint _length_loc, _color_loc;
  GLuint _pos_loc;
  GLuint _vao, _vbo;
  
}; // GLHacker
  
}; // namespace matric2020

#endif // GL_HACKER_HPP
