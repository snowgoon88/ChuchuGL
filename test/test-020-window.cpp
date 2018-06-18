/* -*- coding: utf-8 -*- */

/** 
 * Test basic GLWindow.
 */

#include <iostream>                       // std::cout

#include <gl_window.hpp>
#include <gl_shader.hpp>

// load shaders
GLShader* base_shader;

  // set up data to draw
float vertices[] = {
  // positions         // colors
  0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
  0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
};

unsigned int VBO, VAO;
  
void render()
{
  // render triangles on every point
  base_shader->use();
  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, 3); // ??
}


//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLWindow win( "GL Window", 1128, 752);

  base_shader = new GLShader( "src/shaders/base.v330.glsl",
                              "src/shaders/base.f330.glsl",
                              "src/shaders/triangle.g330.glsl" );
  
  // Create VAO, VBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // position attribute of shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute of shader
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  
  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  // glBindVertexArray(0);

  win.run( render );
  
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
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  // tidy shaders
  delete base_shader;
  
  return 0;
}
