/* -*- coding: utf-8 -*- */

/** 
 * Test basic GLWindow.
 * Display a rotating cube with different triangle colors.
 * and an "axes" in red before, in user field of view
 */

#include <iostream>                       // std::cout

#include <gl_window.hpp>
#include <gl_shader.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// load shaders
GLShader* base_shader;
GLShader* axes_shader;
GLWindow* _win;

float axes_vertices[] = {
  // positions            // color
  0.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
};

// set up data to draw
float cube_vertices[] = {
  // positions            // color
  0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f, // face x+ RG
  0.5f, 0.5f, 0.5f,       1.0f, 0.0f, 0.0f,
  0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 0.0f,
  0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
  0.5f, 0.5f, -0.5f,      0.0f, 1.0f, 0.0f,
  0.5f, 0.5f, 0.5f,       0.0f, 1.0f, 0.0f,

  0.5f, 0.5f, -0.5f,      0.0f, 0.0f, 1.0f, // face y+ GB
  -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,
  0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,
  0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 0.0f,
  -0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 0.0f,
  -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 0.0f,

  -0.5f, 0.5f, -0.5f,      1.0f, 0.0f, 1.0f, // face x- GR
  -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 1.0f,
  -0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 1.0f,
  -0.5f, 0.5f, -0.5f,     0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 1.0f,

  -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f, // face y- BG
  0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,
  -0.5f,- 0.5f, 0.5f,     0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
  0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
  0.5f, -0.5f, 0.5f,      0.0f, 1.0f, 0.0f,

  0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f, // face z+ BR
  0.5f, 0.5f, 0.5f,       0.0f, 0.0f, 1.0f,
  -0.5f, 0.5f, 0.5f,      0.0f, 0.0f, 1.0f,
  0.5f, -0.5f, 0.5f,      1.0f, 0.0f, 0.0f,
  -0.5f, 0.5f, 0.5f,      1.0f, 0.0f, 0.0f,
  -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f,

  0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f, // face z- RB
  -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
  -0.5f, 0.5f, -0.5f,     1.0f, 0.0f, 0.0f,
  0.5f, -0.5f, -0.5f,     1.0f, 0.5f, 1.0f,
  -0.5f, 0.5f, -0.5f,     1.0f, 0.5f, 1.0f,
  0.5f, 0.5f, -0.5f,      1.0f, 0.5f, 1.0f
};
  
  
float vertices[] = {
  // positions         // colors
  0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
  0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
};

unsigned int cube_vbo, cube_vao;
unsigned int axe_vbo, axe_vao;

GLuint proj_view_loc, proj_loc, model_loc;
  
void render()
{
  // render triangles on every point
  base_shader->use();

  // create transformations
  glm::mat4 view(1.0f);
  glm::mat4 projection(1.0f);
  glm::mat4 model(1.0f);
  projection = glm::perspective(glm::radians(45.0f),
                                (float) _win->_screen_width / (float) _win->_screen_height,
                                0.1f, 100.0f);
  view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  // rotation
  model = glm::rotate( model,
                       (float) glfwGetTime() * glm::radians(50.0f),
                       glm::vec3( 0.5f, 1.0f, 0.0f) );
  
  // pass transformation matrices to the shader
  glUniformMatrix4fv( proj_view_loc, 1, GL_FALSE, glm::value_ptr(projection*view) );
  //glUniformMatrix4fv( proj_loc, 1, GL_FALSE, glm::value_ptr(projection) );
  glUniformMatrix4fv( model_loc, 1, GL_FALSE, glm::value_ptr(model) );
  
  // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    
  glBindVertexArray(cube_vao);
  glDrawArrays(GL_TRIANGLES, 0, 36); // mode, first, count

  // and now axes
  axes_shader->use();
  glBindVertexArray(axe_vao);
  glDrawArrays(GL_POINTS, 0, 1); // mode, first, count
  
}


//******************************************************************************
int main( int argc, char *argv[] )
{
  // tmp
  glm::mat4 glm_tmp(1.0f); // default is identity
  std::cout << "GLM init matrix" << std::endl;
  std::cout << glm::to_string( glm_tmp) << std::endl;
  
  
  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 1128, 752);

  // base_shader = new GLShader( "src/shaders/base.v330.glsl",
  //                             "src/shaders/base.f330.glsl",
  //                             "src/shaders/triangle.g330.glsl" );
  base_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                              "src/shaders/base3D.frag330.glsl" );
  proj_view_loc = base_shader->getUniformLocation( "proj_view" );
  //proj_loc = base_shader->getUniformLocation( "projection" );
  model_loc = base_shader->getUniformLocation( "model" );

  axes_shader = new GLShader( "src/shaders/base.v330.glsl",
                             "src/shaders/base.f330.glsl",
                             "src/shaders/axes.g330.glsl" );
  
  // Create VAO, VBO
  glGenVertexArrays(1, &cube_vao);
  glGenBuffers(1, &cube_vbo);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(cube_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
               GL_STATIC_DRAW);

  // position attribute of shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute of shader
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenVertexArrays(1, &axe_vao);
  glGenBuffers(1, &axe_vbo);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(axe_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, axe_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), axes_vertices,
               GL_STATIC_DRAW);

  // position attribute of shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute of shader
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  
  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  // glBindVertexArray(0);

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);

  // init timer
  glfwSetTime( 0.0 );
  _win->run( render );
  
  
  // // render window
  // while (!glfwWindowShouldClose(win._window)) {
  //   // update screen size
  //   glfwGetFramebufferSize(win._window, &win._screen_width, &win._screen_height);
      
  //   // TODO Vérifier ce que fait glViewport
  //   glViewport(0, 0, win._screen_width, win._screen_height);
  //   /* Clear the background as black */
  //   glClearColor(0.8f, 0.8f, 0.8f, 1.f);
  //   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    
    
  //   glfwSwapBuffers(win._window);
  //   glfwPollEvents();
  //   }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &cube_vao);
  glDeleteBuffers(1, &cube_vbo);
  glDeleteVertexArrays(1, &axe_vao);
  glDeleteBuffers(1, &axe_vbo);

  // tidy shaders
  delete base_shader;
  delete axes_shader;
  delete _win;
  
  return 0;
}
