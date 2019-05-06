/* -*- coding: utf-8 -*- */

/** 
 * Teste GLTextShaders
 */
// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <gl_text_shaders.hpp>
#include <gl_shader.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// ******************************************************************** Global
GLFWwindow* _window;
int _screen_width=800, _screen_height=600;

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
GLShader* _base3D_shader = NULL;
GLuint _proj_view_loc_3D;
GLuint _model_loc_3D;
GLuint _line_vbo, _line_vao;

void set_shader()
{
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
}
void free_shaders()
{
  glDeleteVertexArrays( 1, &_line_vao);
  glDeleteBuffers( 1, &_line_vbo);
  if (_base3D_shader != NULL) delete _base3D_shader;
}

// ********************************************************************** main
int main( int argc, char *argv[] )
{
  //TODO Ouvrir une fenêtre basique avec GLFWT
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwSetErrorCallback(error_callback);

  _window = glfwCreateWindow(_screen_width, _screen_height,
			     "Essai GLTextShaders", NULL, NULL);
  if (!_window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // set key callback
  glfwSetKeyCallback(_window, key_callback);
  
  /* Make the window's context current */
  glfwMakeContextCurrent(_window);

  glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
  float _sx = 2.0 / _screen_width;
  float _sy = 2.0 / _screen_height;
  
  GLTextShaders gltext;
  gltext.set_size( 16 );

  set_shader();
  
  std::cout << "Blanc en " <<  -1 + 8 * _sx << ", " << 1 - 50 * _sy << std::endl;
  
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(_window))
    {
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      float sx = 2.0 / _screen_width;
      float sy = 2.0 / _screen_height;
      
      /* Render here */
      glClearColor( 0,0,0,1 );
      glClear(GL_COLOR_BUFFER_BIT);

      /* line */
      // create transformations
      glm::mat4 proj_view(1.0f);
      _base3D_shader->use();
      glUniformMatrix4fv( _proj_view_loc_3D, 1, GL_FALSE,
                          glm::value_ptr( proj_view ));
      glUniformMatrix4fv( _model_loc_3D, 1, GL_FALSE,
                          glm::value_ptr( proj_view) );
      glBindVertexArray( _line_vao );
      glDrawArrays( GL_LINES, 0, 2 );
      
      gltext.pre_render();
      gltext.set_color( {1,1,1,1} );
      gltext.set_size( 16 );
      gltext.render_text("Cette phrase est en blanc",
        		 -1 + 8 * sx, 1 - 50 * sy,
        		 sx, sy);
      gltext.set_size( 40 );
      gltext.set_color( {0,0,1,1} );
      gltext.render_text("Celle-la est bleu, enfin normalement...",
        		 -1 + 16 * sx, 1 - 100 * sy,
        		 sx, sy);
      gltext.set_size( 16 );
      gltext.set_color( {1,0,0,1} );
      gltext.render_text("Et hop du rouge énervé",
        		 -1 + 16 * sx, 1 - 150 * sy,
        		 sx, sy);
      gltext.render_text(u"Et hop du rouge énervé",
        		 -1 + 16 * sx, 1 - 180 * sy,
        		 sx, sy);
      
      gltext.set_color( {1,1,1,1} );
      gltext.render_text(u"Centré",
        		 0, 0,
        		 sx, sy);
      gltext.post_render();

      _base3D_shader->use();
      glUniformMatrix4fv( _proj_view_loc_3D, 1, GL_FALSE,
                          glm::value_ptr( proj_view ));
      glUniformMatrix4fv( _model_loc_3D, 1, GL_FALSE,
                          glm::value_ptr( proj_view) );
      glBindVertexArray( _line_vao );
      glDrawArrays( GL_LINES, 0, 2 );

      
      /* Swap front and back buffers */
      glfwSwapBuffers(_window);
      
      /* Poll for and process events */
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}
