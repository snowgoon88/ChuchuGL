/* -*- coding: utf-8 -*- */

/** 
 * Teste GLTextmapShaders
 */
// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <gl_textmap_shaders.hpp>

#include <gl_shader.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// ******************************************************************** Global
GLFWwindow* _window;
int _win_width=800, _win_height=600;

/**
 * callback pour gérer les messages d'erreur de GLFW
 */
static void error_callback(int error, const char* description)
{
  std::cerr <<  description << std::endl;
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // ESC
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(_window, GL_TRUE);
};

// ******************************************************************* graphic
float line_vertices[] = {
  // positions            // color
  -1.f, -1.f, 0.0f,     0.0f, 1.0f, 0.0f,
   1.f,  1.f, 0.0f,     0.0f, 1.0f, 0.0f
};
float axes_vertices[] = {
  // positions            // color
  0.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
};
GLShader* _base3D_shader = nullptr;
GLShader* _axes_shader = nullptr;
GLuint _proj_view_loc_3D;
GLuint _model_loc_3D;
GLuint _line_vbo, _line_vao;
GLuint _axe_vbo, _axe_vao;

void set_shader()
{
  // Axes as a two lines crossing at 0,0
  _axes_shader = new GLShader( "src/shaders/base.v330.glsl",
                               "src/shaders/base.f330.glsl",
                               "src/shaders/axes.g330.glsl" );
  
  _base3D_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                              "src/shaders/base3D.frag330.glsl" );
  _proj_view_loc_3D = _base3D_shader->getUniformLocation( "proj_view" );
  _model_loc_3D = _base3D_shader->getUniformLocation( "model" );

  // LINE vertices
  glGenVertexArrays(1, &_line_vao);
  glGenBuffers(1, &_line_vbo);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(_line_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, _line_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices,
               GL_STATIC_DRAW);

  // position attribute of shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute of shader
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // AXES vertices
  glGenVertexArrays(1, &_axe_vao);
  glGenBuffers(1, &_axe_vbo);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(_axe_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, _axe_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), axes_vertices,
               GL_STATIC_DRAW);

  // reset bindings
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray( 0 );
  
}
void free_shaders()
{
  glDeleteVertexArrays( 1, &_line_vao);
  glDeleteBuffers( 1, &_line_vbo);
  if (_base3D_shader) delete _base3D_shader;
  if (_axes_shader) {
    glDeleteVertexArrays( 1, &_axe_vao);
    glDeleteBuffers( 1, &_axe_vbo);
    delete _base3D_shader;
  }
}

// ********************************************************************** main
int main( int argc, char *argv[] )
{
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwSetErrorCallback(error_callback);

  _window = glfwCreateWindow(_win_width, _win_height,
			     "Essai GLTextmapShaders", nullptr, nullptr);
  if (!_window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // set key callback
  glfwSetKeyCallback(_window, key_callback);
  
  /* Make the window's context current */
  glfwMakeContextCurrent(_window);
  /* Enable blending, necessary for our alpha texture */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  GLTextmapShaders gltext;
  // gltext.set_size( 16 );
  // gltext.build_map();
  std::cout << gltext.str_dump() << std::endl;

  set_shader();
  // create transformations
  glm::mat4 _proj(1.0f);
  _proj = glm::ortho( -1.f, 1.f, // left;right
                      -1.f, 1.f, // bottom,top
                      -1.f, 100.f // near far
                      );
  
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(_window))
    {
      glfwGetFramebufferSize(_window, &_win_width, &_win_height);
      // which part of the window is used
      glViewport( 0, 0, _win_width, _win_height );
      //glViewport(- _win_width / 2, - _win_height / 2,
      //           _win_width / 2, _win_height / 2);
      //float sx = 2.0 / _win_width;
      //float sy = 2.0 / _win_height;
      
      /* Render here */
      glClearColor( 0,0,0,1 );
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // /* line */
      
      // _base3D_shader->use();
      // glUniformMatrix4fv( _proj_view_loc_3D, 1, GL_FALSE,
      //                     glm::value_ptr( _proj ));
      // glUniformMatrix4fv( _model_loc_3D, 1, GL_FALSE,
      //                     glm::value_ptr( _proj ) );
      // glBindVertexArray( _line_vao );
      // glDrawArrays( GL_LINES, 0, 2 );

      // text
      // gltext.pre_render();
      // gltext.set_color( {1,0,1,1} );
      gltext.render_textmap( _proj );

      // and now axes
      _axes_shader->use();
      glBindVertexArray(_axe_vao);
      glDrawArrays(GL_POINTS, 0, 1); // mode, first, count
      
      /* Swap front and back buffers */
      glfwSwapBuffers(_window);
      
      /* Poll for and process events */
      glfwPollEvents();
    }
  
  glfwTerminate();
  free_shaders();
  return 0;
}
