/* -*- coding: utf-8 -*- */

/** 
 * test-002-texture.cpp
 * 
 * Display one Texure with Shaders
 */

#include <gl_window.hpp>

#include <iostream>                       // std::cout

#include <gl_shader.hpp>
#include <SOIL/SOIL.h>               // Load images
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>               // alt to load images

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

GLWindow* _win;

GLShader* _texture_shader = NULL;
GLShader* _base3D_shader = NULL;
GLShader* _basic_shader = NULL;
GLShader* _axes_shader = NULL;
GLuint _texture_loc;
GLuint _proj_view_loc;
GLuint _model_loc;
GLuint _proj_view_loc_3D;
GLuint _model_loc_3D;
GLuint _texture_id;
GLuint _color_loc;
GLuint _dummy_loc;
GLuint _vao, _vbo;
GLuint _axe_vbo, _axe_vao;
GLuint _line_vbo, _line_vao;

float vertices_tex[] = {
  // x,y,z  s,t
  -0.5f, -0.5f, 0.f,        0.f, 0.f,
  0.5f, -0.5f, 0.f,         1.0f, 0.f,
  0.5f, 0.5f, 0.f,          1.0f, 1.0f,
  
  0.5f, 0.5f, 0.f,          1.0f, 1.0f,
  -0.5f, 0.5f, 0.f,         0.f, 1.0f,
  -0.5f, -0.5f, 0.f,        0.f, 0.f
};
int vertices_size = 6*(3+2) * sizeof( GL_FLOAT );

float axes_vertices[] = {
  // positions            // color
  0.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
};

float line_vertices[] = {
  // positions            // color
  -0.5f, 0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
  0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 0.0f
};

void set_shaders()
{
  // _texture_shader = new GLShader( "src/shaders/texture.v330.glsl",
  //                                 "src/shaders/texture.f330.glsl" );
  // _proj_view_loc = _texture_shader->getUniformLocation( "proj_view" );
  
  
  _basic_shader = new GLShader( "src/shaders/basic.v330.glsl",
                                "src/shaders/basic.f330.glsl" );
  _proj_view_loc = _basic_shader->getUniformLocation( "proj_view" );
  _model_loc = _basic_shader->getUniformLocation( "model" );
  _texture_loc = _basic_shader->getUniformLocation( "texture_id" );
  _color_loc = _basic_shader->getUniformLocation( "color_uni" );
  //_dummy_loc = _basic_shader->getUniformLocation( "coloring" );
  std::cout << "  basic pv=" << _proj_view_loc;
  std::cout << " m=" << _model_loc;
  std::cout << " t=" << _texture_loc;
  std::cout << " c=" << _color_loc << std::endl;

  _axes_shader = new GLShader( "src/shaders/base.v330.glsl",
                               "src/shaders/base.f330.glsl",
                               "src/shaders/axes.g330.glsl" );

  _base3D_shader = new GLShader( "src/shaders/base3D.vert330.glsl",
                              "src/shaders/base3D.frag330.glsl" );
  _proj_view_loc_3D = _base3D_shader->getUniformLocation( "proj_view" );
  _model_loc_3D = _base3D_shader->getUniformLocation( "model" );
  std::cout << "  base3D pv=" << _proj_view_loc_3D;
  std::cout << " m=" << _model_loc_3D << std::endl;

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

  // position attribute of shader
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute of shader
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);


  
  // set vertex data
  // 
  glGenVertexArrays( 1, &_vao );
  glGenBuffers( 1, &_vbo );

  glBindVertexArray( _vao );

  glBindBuffer( GL_ARRAY_BUFFER, _vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_tex), vertices_tex,
                GL_STATIC_DRAW );

  // position attribute
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                         (3+2) * sizeof(float), // stride
                         (void*)0); // offset
  glEnableVertexAttribArray(0);
  // texture coordinate
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE,
                         (3+2) * sizeof(float), // stride
                         (void*)(3 * sizeof(float)) ); // offset
  glEnableVertexAttribArray(1);

  // Textures
  // std::cout << "__IMAGE" << std::endl;
  std::string filename{"data/stripe_yel_80x80.png"};
  // std::string filename{"data/tex_titlerocket.png"};
  // std::cout << "__TEXTURE avant=" << _texture_id  << std::endl;
  // glGenTextures( 1, &_texture_id);
  // glBindTexture( GL_TEXTURE_2D, _texture_id );
  // std::cout << "  après=" << _texture_id << std::endl;
  // // set wrapping parameters (repeat, usual default)
  // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  // // set filtering
  // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  // // load image, create texture and generate mipmap
  // int img_w, img_h, img_nb_channels;
  // stbi_set_flip_vertically_on_load( true ); // flip image on y-axis
  // unsigned char *img_data = stbi_load( filename.c_str(),
  //                                      &img_w, &img_h, &img_nb_channels, 0 );
  // std::cout << "  loaded " << img_w << "x" << img_h << " nb=" << img_nb_channels << std::endl;
  // if (img_data) {
  //   glTexImage2D( GL_TEXTURE_2D, 0,
  //                 GL_RGBA, img_w, img_h, 0,
  //                 GL_RGBA, GL_UNSIGNED_BYTE, img_data );
  //   glGenerateMipmap( GL_TEXTURE_2D );
  // }
  // else {
  //   std::cerr << "GLSprite: STB loading error:  (" << filename << ")" << std::endl;
  // }
  
  // stbi_image_free(img_data);

  // Charger la texture
  //glActiveTexture(GL_TEXTURE0);
  _texture_id = SOIL_load_OGL_texture
    (
       filename.c_str(),                         // pathfile
       SOIL_LOAD_AUTO,                           // 
       SOIL_CREATE_NEW_ID,                       //
       SOIL_FLAG_INVERT_Y                        // Corrige les Y upside/down
       );
    if(_texture_id == 0)
      std::cerr << "GLSprite: SOIL loading error: '" << SOIL_last_result() << "' (" << filename << ")" << std::endl;
}

void free_shaders()
{
  glDeleteVertexArrays( 1, &_vao);
  glDeleteBuffers( 1, &_vbo);
  glDeleteTextures( 1, &_texture_id);
  if (_texture_shader != NULL) delete _texture_shader;
  if (_basic_shader != NULL) delete _basic_shader;
  if (_base3D_shader != NULL) delete _base3D_shader;
}

void render()
{
  // create transformations
  glm::mat4 view(1.0f);
  glm::mat4 projection(1.0f);
  glm::mat4 model(1.0f);
  
  //projection = glm::ortho( -2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f );
  //view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  //projection = glm::mat4( 1.0f );
  //view = glm::mat4( 1.0f );

  // LINE
  model = glm::rotate( model,
                       (float) glfwGetTime() * glm::radians(50.0f),
                       glm::vec3( 0.0f, 0.0f, 1.0f) );


  _base3D_shader->use();
  glUniformMatrix4fv( _proj_view_loc_3D, 1, GL_FALSE, glm::value_ptr(projection*view) );
  glUniformMatrix4fv( _model_loc_3D, 1, GL_FALSE, glm::value_ptr(model) );
  glBindVertexArray( _line_vao );
  glDrawArrays( GL_LINES, 0, 2 );

  
  // activate program
  //_texture_shader->use();
  projection = glm::ortho( -2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f );
  view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  model = glm::mat4( 1.0f );
  _basic_shader->use();
  // pass transformation matrices to the shader
  glUniformMatrix4fv( _proj_view_loc, 1, GL_FALSE,
                      glm::value_ptr(projection*view) );
  glUniformMatrix4fv( _model_loc, 1, GL_FALSE,
                      glm::value_ptr(model) );

  // tmp
  // std::cout << "  proj_view " << std::endl;
  // std::cout << glm::to_string( projection * view ) << std::endl;
  // std::cout << "  model " << std::endl;
  // std::cout << glm::to_string( model ) << std::endl;
  
  // tell what texture => TEXTURE0
  // TODO could be done once, before
  //glUniform1i( _texture_loc, _texture_id );
  // BIG QUESTION : why not NEEDED ????????????????????????????????????????????

  // color
  glUniform3f( _color_loc, 0.0f, 0.0f, 1.0f ); 

  // bind textures on corresponding texture units
  //glActiveTexture(GL_TEXTURE0); // Maybe beforeglUniformli ??????????????????
  glBindTexture(GL_TEXTURE_2D, _texture_id);

  // draw
  glBindVertexArray( _vao );
  glDrawArrays( GL_TRIANGLES, 0, 6 ); // mode, first, count
  //glDrawArrays( GL_LINES, 0, 2);

   // and now axes
  _axes_shader->use();
  glBindVertexArray(_axe_vao);
  glDrawArrays(GL_POINTS, 0, 1); // mode, first, count

}

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  _win = new GLWindow( "GL Window", 640, 480);

  std::cout << "__SHADERS" << std::endl;
  set_shaders();

  std::cout << "__RENDER" << std::endl;
  _win->run( render );

  std::cout << "__FREE memory" << std::endl;
  delete _win;
  free_shaders();
  
  return 0;
}
