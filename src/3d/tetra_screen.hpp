/* -*- coding: utf-8 -*- */

#ifndef TETRA_GLSCREEN_HPP
#define TETRA_GLSCREEN_HPP

/** 
 * Implements GL3DSScreen and draw a tetrahedron.
 *
 * Use the GL3DCanvas and its 'smart_shader'.
 *
 * Uniforms value are stored by the GLSL program : no need to set them
 *   at every rendering iteration.
 *
 * Render a Tetrahedron with colored faces and outlined edges.
 * Vertex and their color are stored in VTX VBO
 * Indexes to draw lines along the edges are stored in ELEMENT VBO
 * Both VBO are binded to a VAO, with the Attrib Location of pos and color.
 * thus -> draw faces with glDrawArrays
 *      -> draw edges with glDrawElements
 *
 * Key event mostly change rendering parameters.
 * Mouse events (button, move and scroll) are transferred to a Trackball
 * via the 'GL3DCanvas'
 */

#include <iostream>                       // std::cout
#include <3d/gl_3dcanvas.hpp>
#include <3d/gl_3dscreen.hpp>

// ***************************************************************************
// *************************************************************** TetraScreen
// ***************************************************************************
class TetraScreen : public GL3DScreen
{
public:
  TetraScreen( GL3DCanvas& canvas ) : GL3DScreen(),
    _canvas( canvas ),
    _model( 1.0f ),
    _mode_param( 0 ), _fade_param( 1.f ),
    _model_rotate_param( false ), _outline_param( false )
  {
    build_buffers();
    build_axes();
    build_help_msg();
  }
  // ***************************************************** TetraScreen::render
  virtual void render( int win_width, int win_height )
  {
    //std::cout << "__RENDER TetraScreen" << std::endl;
    glPushAttrib (GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);
  
    // SHOULD be available somewhere, in Engine ?
    //glfwGetFramebufferSize(_win->wind
    _canvas.render_viewport( 0, 0, win_width, win_height );
    // TODO: could add default color

    // Rotate our model 
    if (_model_rotate_param) {
      _model = glm::rotate( _model,
                            glm::radians(1.0f),
                            glm::vec3( 0.5f, 1.0f, 0.0f) );
    }

    // view_projection transform
    auto projection = glm::ortho( -2.f, 2.f, // left;right
                                  -2.f, 2.f, // bottom,top
                                  100.f, -100.f // near far
                                  );
    auto vp_track = projection * _canvas.get_trackball_transform();
    // // Invert Z axis as OpenGL is left-handed
    glm::mat4 z_inverter = glm::scale( glm::mat4(1.0f),
                                       glm::vec3( 1, 1, -1 ));
    auto proj_view = vp_track * z_inverter;

    // Tetrahedron
    // TODO: Some call could be made/set outside of render (proj)
    // _smart_shader->use();
    _canvas.smart_shader()->use();
    // set uniforms
    //glUniform1i( _u_mode, 1 );        // 1=uniform color
    //glUniform1f( _u_fade, 1.f );        // 0=uniform color
    // default model, so not passed
    // glUniformMatrix4fv( _u_model, 1, GL_FALSE,
    //                   glm::value_ptr(_model) );
    // glUniformMatrix4fv( _u_pv, 1, GL_FALSE,
    //                   glm::value_ptr(proj_view) );
    _canvas.set_projection_view_transform( proj_view );
    _canvas.set_model_transform( _model );
    
    // uniform color
    //glUniform3f( _u_color, 0.f, 0.5f, 0.5f );
    _canvas.set_color_rgb( {0.f, 0.5f, 0.5f}, false /*no force_use*/);

    // draw faces
    glBindVertexArray( _tetra_faces_vao );
    glDrawArrays( GL_TRIANGLES, 0, 4*3 /*nb vtx*/); // mode, first, count

    if (_outline_param) {
      // draw sides in white, little outside
      // glUniform3f( _u_color, 1.f, 1.f, 1.f );
      // glUniform1i( _u_mode, 1); // use unicolor
      _canvas.set_param( 1, 1.f, false );
      _canvas.set_color_rgb( {1.f, 1.f, 1.f} );
      glDrawElements( GL_LINES, 6*2, GL_UNSIGNED_SHORT, 0);
      // back to blue
      // glUniform3f( _u_color, 0.f, 0.5f, 0.5f );
      _canvas.set_param( _mode_param, _fade_param );
    }
  
    // and now axes
    _axes_shader->use();
    glBindVertexArray( _axe_vao);
    glDrawArrays( GL_POINTS, 0, 1); // mode, first, count

    // Precaution : ubind VAO
    // WARNING : need to unbind our VAO, see below !
    glBindVertexArray( 0 );
    // WARNING : GL3DTextshaders does not use VAO, so it binds
    //           its VBO to the CURRENT VAO...
    //           => Need to unbind our VAO before using _text_shader !!!!!

    // TODO: Interace in Canvas ?
    // Now some text
    auto text_shader = _canvas.text_shader();
    text_shader->set_scale( (5.f)/(float)win_width,
                           (5.f)/(float)win_height );
    // upper text
    GLfloat ypos = 1.9f;
    text_shader->pre_render( projection );
    text_shader->set_color( {1.f, 1.f, 1.f, 1.f} );
    for( auto& msg: _help_msg) {
      text_shader->render( msg, -1.9f, ypos );
      ypos -= 0.1;      
    }
    text_shader->post_render();
  
    glPopAttrib();
    glUseProgram(0);
  }
  // ****************************************************** TetraScreen::build
  void build_buffers()
  {
    const GLfloat S = 1.f/11.f;
    const GLfloat T = 1.f/3.f;

    // Build a Tetrahedron, with 4 different colors
    const GLfloat RADIUS = 1.f/sqrtf( 3.f );
    const GLfloat ANGLE = (float) M_PI * 2.f / 3.f;
    const GLfloat HEIGHT = sqrtf( 2.f/3.f );

    GLfloat tetrafaces_vtx[] = {
      // 3D positions                   R,G,B
      // First face
      RADIUS, 0.f, 0.f,                                           1.f, 0.f, 0.f,
      RADIUS*cosf(ANGLE), RADIUS*sinf(ANGLE), 0.f,                1.f, 0.f, 0.f,
      0.f, 0.f, HEIGHT,                                           1.f, 0.f, 0.f,

      // Second face
      RADIUS*cosf(ANGLE), RADIUS*sinf(ANGLE), 0.f,                0.f, 1.f, 0.f,
      RADIUS*cosf(2.f*ANGLE), RADIUS*sinf(2.f*ANGLE), 0.f,        0.f, 1.f, 0.f,
      0.f, 0.f, HEIGHT,                                           0.f, 1.f, 0.f,

      // Third face
      RADIUS*cosf(2.f*ANGLE), RADIUS*sinf(2.f*ANGLE), 0.f,        0.f, 0.f, 1.f,
      RADIUS, 0.f, 0.f,                                           0.f, 0.f, 1.f,
      0.f, 0.f, HEIGHT,                                           0.f, 0.f, 1.f,

      // Bottom face
      RADIUS, 0.f, 0.f,                                           1.f, 1.f, 0.f,
      RADIUS*cosf(2.f*ANGLE), RADIUS*sinf(2.f*ANGLE), 0.f,        1.f, 1.f, 0.f,
      RADIUS*cosf(1.f*ANGLE), RADIUS*sinf(1.f*ANGLE), 0.f,        1.f, 1.f, 0.f,

    };

    // lines along sides as indexes
    GLushort tetrasides_idx[] = {
      // first face
      0,1, 1,2, 2,0,
      // second face : only 2 more
      3,4, 4,5,
      // third face : only one more
      6,7 
      // bottom face => nothing left
    };

    // Make Data -> Tetrahedron
    // Now a VAO for faces and lines (using ELEMENTS)
    glGenVertexArrays( 1, &_tetra_faces_vao );
    glBindVertexArray( _tetra_faces_vao );
    // A VBO with vertex data (pos and color)
    glGenBuffers( 1, &_tetra_faces_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, _tetra_faces_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(tetrafaces_vtx), tetrafaces_vtx,
                  GL_STATIC_DRAW );

    // A VBO with indexes data for lines along sides
    glGenBuffers( 1, &_tetra_idxsides_vbo );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _tetra_idxsides_vbo );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetrasides_idx),
  		 tetrasides_idx, GL_STATIC_DRAW);

    std::cout << "__TETRA vao=" << _tetra_faces_vao;
    std::cout << " face_vbo=" << _tetra_faces_vbo << std::endl;
    std::cout << " side_idx=" << _tetra_idxsides_vbo << std::endl;

    // position attribute
    glVertexAttribPointer( _canvas.vertex_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)0); // offset
    glEnableVertexAttribArray( _canvas.vertex_attribut() );
    // texture coordinate
    glVertexAttribPointer( _canvas.color_attribut(), 3, GL_FLOAT, GL_FALSE,
                           (3+3) * sizeof(float), // stride
                           (void*)(3 * sizeof(float)) ); // offset
    glEnableVertexAttribArray( _canvas.color_attribut() );

    // Unbind VAO
    // TODO: test without unbinding...
    glBindVertexArray( 0 );
  }
  void build_axes()
  {
    _axes_shader = new GLShader( "src/shaders/base.v330.glsl",
                                 "src/shaders/base.f330.glsl",
                                 "src/shaders/axes.g330.glsl" );

    float axes_vertices[] = {
      // positions            // color
      0.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
    };
    
    // Make DATA -> Axes
    glGenVertexArrays(1, &_axe_vao);
    glGenBuffers(1, &_axe_vbo);
    std::cout << "__AXES vao=" << _axe_vao;
    std::cout << " vbo=" << _axe_vbo << std::endl;
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray( _axe_vao );
  
    glBindBuffer(GL_ARRAY_BUFFER, _axe_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), axes_vertices,
                 GL_STATIC_DRAW);

    // position attribute of shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute of shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }
  void build_help_msg()
  {
    _help_msg.push_back( "M1:rot, shift+M1: trans, SCROLL: zoom.   r/R: reset camera");
    _help_msg.push_back( "TO UPDATE : key to change parameters" );
    update_help_msg();
  }
  void update_help_msg()
  {
     std::stringstream help_msg;
     help_msg << "m/M: mode (" << _mode_param << ")";
     help_msg << ", f/F: fade (" << _fade_param << ")";
     help_msg << ", s/S: stopRot (" << (not _model_rotate_param) << ")";
     help_msg << ", o/O: outline (" << _outline_param << ")";
  
     _help_msg[1] = help_msg.str(); 
  }

  // ************************************************** TetraScreen::attributs
protected:
  // VAOs and VBOs
  GLuint _tetra_faces_vao, _tetra_faces_vbo, _tetra_idxsides_vbo;
  GL3DCanvas& _canvas;

  GLuint _axe_vbo, _axe_vao;
  GLShader* _axes_shader;
  
  // Rendering parameters
  glm::mat4 _model;
  GLint     _mode_param;
  GLfloat   _fade_param;
  bool      _model_rotate_param;
  bool      _outline_param;

  // Help message
  std::vector<std::string> _help_msg;
public:
  // *************************************************** TetraScreen::callback
  virtual void key_callback(int key, int scancode, int action, int mods)
  {
    //std::cout << "key=" << key << std::endl;
    
    // R
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      _canvas.reset_trackball();
    }
    // M
    else if (key == GLFW_KEY_SEMICOLON && action == GLFW_PRESS) {
      _mode_param = 1 - _mode_param;
      //std::cout << "__PARAM mode=" << _mode_param << std::endl;
      _canvas.set_param( _mode_param, _fade_param, true /*force*/ );
      update_help_msg();
    }
    // F
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
      _fade_param -= 1.f/3.f;
      if (_fade_param < -0.1f) _fade_param = 1.f;
      //std::cout << "__PARAM fade=" << _fade_param << std::endl;
      _canvas.set_param( _mode_param, _fade_param, true /*force*/ );
      update_help_msg();
    }
    // S
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      _model_rotate_param = not _model_rotate_param;
      //std::cout << "__PARAM model_rotate=" << _model_rotate_param << std::endl;
      update_help_msg();
    }
    // O
    else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
      _outline_param = not _outline_param;
      //std::cout << "__PARAM outline=" << _outline_param << std::endl;
      update_help_msg();
    }
  }
  virtual void mouse_button_callback( int button, int action, int mods,
                                      double xpos, double ypos,
                                      int win_width, int win_height )
  {
    _canvas.mouse_button_trackball( button, action, mods,
                                    xpos/win_width, ypos/win_height );
  }
  virtual void mouse_move_callback( double xpos, double ypos,
                                    int win_width, int win_height )
  {
    _canvas.mouse_move_trackball( xpos/win_width, ypos/win_height );
  }
  virtual void mouse_scroll_callback( double xoffset, double yoffset )
  {
    _canvas.mouse_scroll_trackball( xoffset, yoffset );
  }
}; // TetraScreen

#endif // TETRA_GLSCREEN_HPP
