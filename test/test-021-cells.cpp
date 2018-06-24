/* -*- coding: utf-8 -*- */

/** 
 * Test basic GLWindow.
 * Display two rotating square with dynamic sides (geometry)
 * and an "axes" in red before, in user field of view
 */

#include <iostream>                       // std::cout

#include <gl_window.hpp>
#include <gl_shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// load shaders
GLShader* cell_shader;
GLShader* axes_shader;
GLWindow* _win;

float axes_vertices[] = {
  // positions            // color
  0.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
};

// set up data to draw
float cell_vertices[] = {
  // positions            // color
  0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 0.0f, // face x+ RG
  0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,

  0.5f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,
  -0.5f, 0.5f, 0.0f,      0.0f, 1.0f, 0.0f,

  -0.5f, 0.5f, 0.0f,      0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,

  -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 1.0f,
  0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 1.0f,
};
  
  
float vertices[] = {
  // positions         // colors
  0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f,
  0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 1.0f,
  -0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 1.0f,
};

unsigned int cell_vbo, cell_vao;
unsigned int axe_vbo, axe_vao;

GLuint proj_view_loc, proj_loc, model_loc;
GLuint c_length_loc;

void render()
{
  // render cell
  cell_shader->use();

  // create transformations
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 model;
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
  double cell_length = sin( glfwGetTime()*5.0 ) * 0.1 + 0.25; 
  glUniform1f( c_length_loc, (float) cell_length );
  
  glBindVertexArray(cell_vao);
  glDrawArrays(GL_POINTS, 0, 4); // mode, first, count
  //glDrawArrays(GL_LINES, 0, 8); // mode, first, count

  // and now axes
  axes_shader->use();
  glBindVertexArray(axe_vao);
  glDrawArrays(GL_POINTS, 0, 1); // mode, first, count
  
}


//******************************************************************************
int main( int argc, char *argv[] )
{
  // tmp
  glm::mat4 glm_tmp; // default is identity
  std::cout << "GLM init matrix" << std::endl;
  std::cout << glm::to_string( glm_tmp) << std::endl;
  
  
  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 1128, 752);

  cell_shader = new GLShader( "src/shaders/cell.v330.glsl",
                              "src/shaders/base.f330.glsl",
                              "src/shaders/cell.g330.glsl" );
  proj_view_loc = cell_shader->getUniformLocation( "proj_view" );
  model_loc = cell_shader->getUniformLocation( "model" );
  c_length_loc = cell_shader->getUniformLocation( "c_length" );
  
  axes_shader = new GLShader( "src/shaders/base.v330.glsl",
                             "src/shaders/base.f330.glsl",
                             "src/shaders/axes.g330.glsl" );

  // Create VAO, VBO
  glGenVertexArrays(1, &cell_vao);
  glGenBuffers(1, &cell_vbo);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(cell_vao);
  
  glBindBuffer(GL_ARRAY_BUFFER, cell_vbo);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(cell_vertices), cell_vertices,
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
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
  glDeleteVertexArrays(1, &cell_vao);
  glDeleteBuffers(1, &cell_vbo);
  glDeleteVertexArrays(1, &axe_vao);
  glDeleteBuffers(1, &axe_vbo);

  // tidy shaders
  delete cell_shader;
  delete axes_shader;
  delete _win;
  
  return 0;
}
