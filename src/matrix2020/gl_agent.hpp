/* -*- coding: utf-8 -*- */

#ifndef GL_AGENT_HPP
#define GL_AGENT_HPP

/** 
 * OpenGL to render Agent
 */

#include <matrix2020/m_scene.hpp>

#include <gl_shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#define AGENT_LENGTH (0.40f)

namespace matrix2020 {
// ***************************************************************************
// ******************************************************************* GLAgent
// ***************************************************************************
class GLAgent
{
  using Vertex = struct s_Vertex {
    GLfloat x, y, z; // pos
  };
private:
  // max number of agent to draw
  static const unsigned int _max_size = 10;
public:
  // ******************************************************* GLAgent::creation
  GLAgent( const GameObjectPtrC& agents ) : _agents(agents)
  {
    _shader = new GLShader( "src/shaders/pos_geom_mvp.v330.glsl",
                            "src/shaders/unicol.f330.glsl",
                            "src/shaders/arrow.g330.glsl" );
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
    std::cout << "GLAgent array=" << _vao << " buff=" << _vbo << std::endl;
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
  // ******************************************************** GLAgent::destroy
  virtual ~GLAgent()
  {
    glDeleteVertexArrays( 1, &_vao);
    glDeleteBuffers( 1, &_vbo);
    delete _shader;
  }
  // ********************************************************* GLAgent::render
  void render( const glm::mat4& projview )
  {
    // create transformations
    glm::mat4 model( 1.0f );
    glm::mat4 identity( 1.0f );
    
    // activate program
    _shader->use();
    glBindVertexArray( _vao );

    // length and color
    glUniform1f( _length_loc, AGENT_LENGTH );
    glUniform3f( _color_loc, 0.0f, 1.0f, 0.0f );
    
    // pass transformation matrices to the shader
    glUniformMatrix4fv( _proj_view_loc, 1, GL_FALSE,
                        glm::value_ptr( projview ) );

    // ori position
    glUniform3f( _pos_loc, 0.f, 0.F, 0.f );
    
    // draw every agent
    for( auto& ag: _agents) {
      // rotation
      model = glm::translate( identity,
                              glm::vec3( ag->pos().x, ag->pos().y, 0.f )) *
        glm::rotate(identity,
                    angle_from_dir( ag->orient() ),
                    glm::vec3( 0.f, 0.f, 1.0f));
      glUniformMatrix4fv( _model_loc, 1, GL_FALSE,
                          glm::value_ptr(model) );

      glDrawArrays(GL_POINTS, 0, 1); // mode, first, count
    }
  }
  // ****************************************************** GLAgent::attribut
  const GameObjectPtrC& _agents;
  GLShader* _shader;
  GLuint _proj_view_loc, _model_loc;
  GLuint _length_loc, _color_loc;
  GLuint _pos_loc;
  GLuint _vao, _vbo;
  
}; // GLAgent
}; // namespace matrix2020

#endif // GL_AGENT_HPP
