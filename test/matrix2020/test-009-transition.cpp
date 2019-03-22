/* -*- coding: utf-8 -*- */

/** 
 * test-009-transition.cpp
 * 
 * Display TransitionFunction of order 1,2,3,4
 */

#include <vector>

#include <gl_window.hpp>
#include <gl_shader.hpp>

#include <transition_func.hpp>
//using namespace matrix2020;

#include <iostream>                       // std::cout

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using Pos2D = glm::vec2;
using Vertex = struct s_Vertex {
  GLfloat x, y, z; // pos
  GLfloat r, g, b; // color
};
using VertexV = std::vector<Vertex>;
using Color = struct {
    GLfloat r,g,b;
};

GLWindow* _win;
glm::mat4 _proj, _view, _projview;

GLShader* _line_shader;
GLuint _proj_view_loc_line;
GLuint _model_loc_line;
GLuint _line_vao, _line_vbo;
VertexV _lines;


void render()
{
  glDrawArrays(GL_LINES, 0, _lines.size() ); // mode, first, count
}

void add_smoothstep( double a, double b, int order, Color col={1.f, 1.f, 1.f})
{
  TransitionFunction tf( a, b, /*a, b*/
                         1.0, /*delta_t*/
                         order );
  std::cout << "  generating points" << std::endl;
  std::cout << tf.str_dump() << std::endl;
  _lines.push_back( Vertex{ (float) a, 0.f, 0.f,    col.r, col.g, col.b} );
  for (double t=0.f; t < 1.f; t += 0.05f) {
    _lines.push_back( Vertex{ (float) t, (float) tf.val(t), 0.f, col.r,col.g,col.b} );
    _lines.push_back( Vertex{ (float) t, (float) tf.val(t), 0.f, col.r,col.g,col.b} );
  }
  _lines.push_back( Vertex{ (float) b, 1.f, 0.f,    col.r,col.g,col.b} );
}

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__TRANSITION FUNCTIONS" << std::endl;
  _lines.clear();
  add_smoothstep( 0.0, 1.0 , 1);
  add_smoothstep( 0.0, 1.0 , 2, {1.f, 0.f, 0.f});
  add_smoothstep( 0.0, 1.0 , 3, {0.f, 0.f, 1.f});
  add_smoothstep( 0.0, 1.0 , 4, {0.f, 1.f, 0.f});

  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 640, 480);

  // Lines with Colors
  _line_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                               "src/shaders/base3D.frag330.glsl" );
  _proj_view_loc_line = _line_shader->getUniformLocation( "proj_view" );
  _model_loc_line = _line_shader->getUniformLocation( "model" );

  glGenVertexArrays( 1, &_line_vao );
  glGenBuffers( 1, &_line_vbo );
  glBindVertexArray( _line_vao );

  glBindBuffer( GL_ARRAY_BUFFER, _line_vbo );
  // glBufferData( GL_ARRAY_BUFFER, sizeof(square_vtx), square_vtx,
  //               GL_STATIC_DRAW );
  glBufferData( GL_ARRAY_BUFFER,
                _lines.size() * sizeof(Vertex),
                _lines.data(),
                GL_STATIC_DRAW);

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

  // draw sinks using lines
  _line_shader->use();
  
  // Camera mode : static
  _proj = glm::mat4( 1.0f );
  _proj = glm::ortho( -1.f, 2.f, // left;right
                      -1.f, 2.f, // bottom,top
                      -1.f, 100.f // near far
                      );
  _view = glm::mat4( 1.0f );
  _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -10.0f));
  _projview = _proj * _view;
  // create transformations
  glm::mat4 model( 1.0f );
  // pass transformation matrices to the shader
  glUniformMatrix4fv( _proj_view_loc_line, 1, GL_FALSE,
                      glm::value_ptr( _projview ) );
  glUniformMatrix4fv( _model_loc_line, 1, GL_FALSE,
                      glm::value_ptr(model) );
  
  glBindVertexArray(_line_vao);

  _win->run( render );

  delete _win;
  
  return 0;
}
