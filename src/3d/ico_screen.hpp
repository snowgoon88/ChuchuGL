/* -*- coding: utf-8 -*- */

#ifndef ICO_GLSCREEN_HPP
#define ICO_GLSCREEN_HPP

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
#include <3d/gl_3dcanvas_screen.hpp>

// ***************************************************************************
// ***************************************************************** IcoScreen
// ***************************************************************************
class IcoScreen : public GL3DCanvasScreen
{
public:
  // ************************************************** IcoScreen::constructor
  IcoScreen( GL3DCanvas& canvas ) : GL3DCanvasScreen( canvas ),
    _model( 1.0f ),
    _mode_param( 0 ), _fade_param( 1.f ),
    _model_rotate_param( false ), _outline_param( false )
  {
    build_faces_and_lines( true );
    build_axes();
    build_help_msg();
    _canvas.set_param( _mode_param, _fade_param, true /*forced*/ );
  }
  // *************************************************** IcoScreen::destructor
  virtual ~IcoScreen()
  {
    _canvas.delete_vao( _ico_faces_vao, _ico_faces_vbo, _ico_idxsides_vbo );
    delete _axes_shader;
  }
  // ***************************************************** IcoScreen::render
  virtual void render( int win_width, int win_height )
  {
    //std::cout << "__RENDER IcoScreen";
    //std::cout << " " << win_width << " x " << win_height  << std::endl;
    glPushAttrib (GL_ENABLE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);
  
    // Fill with BackgroundColor
    _canvas.render_viewport( 0, 0, win_width, win_height );
    
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
    _canvas.set_projection_view_transform( proj_view );
    _canvas.set_model_transform( _model );

    // uniform color
    //glUniform3f( _u_color, 0.f, 0.5f, 0.5f );
    _canvas.set_color_rgb( {0.f, 0.5f, 0.5f}, false /*no force_use*/);

    // draw faces
    glBindVertexArray( _ico_faces_vao );
    glDrawArrays( GL_TRIANGLES, 0, 20*3 /*nb vtx*/); // mode, first, count

    if (_outline_param) {
      // draw sides in white, little outside
      _canvas.set_param( 1, 1.f, false );
      _canvas.set_color_rgb( {1.f, 1.f, 1.f} );
      glDrawElements( GL_LINES, 30*2, GL_UNSIGNED_INT, 0);
      // back to param
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
  // ************************************************** IcoScreen::attributs
protected:
  // VAOs and VBOs
  GLuint _ico_faces_vao, _ico_faces_vbo, _ico_idxsides_vbo;

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
  // ****************************************************** IcoScreen::build
protected:
  const float V_ANGLE = atanf( 0.5f );
  const float H_ANGLE = 2.0 * M_PI / 5.0;

  using VTX = GLfloat;
  using VTXvec = std::vector<VTX>;
  
  using Color = std::vector<GLfloat>;
  const std::vector<Color> _color_vec = {
    {0,0,0} /*black*/,      {1,1,1} /*white*/,   {1,0,0} /*red*/,
    {0,1,0} /*lime*/,       {0,0,1} /*blue*/,    {1,1,0} /*yellow*/,
    {0,1,1} /*cyan*/,       {1,0,1} /*magenta*/, {0.75f,0.75f, 0.75f} /*silver*/,
    {0.5f,0.5f,0.5f} /*gray*/, {0.5f,0,0} /*maroon*/,  {0.5f,0.5f,0} /*olive*/,
    {0,0.5f,0} /*green*/,    {0.5f,0,0.5f} /*purple*/,{0,0.5f,0.5f} /*teal*/,
    {0,0,0.5f} /*navy*/,    {1,0.65f,0} /*orange*/, {0,1,0.65f} /*spring green*/,
    {0.65f,0,1} /*violet*/, {0.95f,0.9f,0.55f} /*khaki*/, {0.87f,1,1} /*l-cyan*/,
    {1,0.98f,0.8f} /*lemon chiffon*/
  };
  VTXvec build_vertex( bool verb=false)
  {
    if (verb) {
      std::cout << "__BUILD vertex for GL3DIcosphere" << std::endl;
    }
    
    VTXvec ico_vtx(12 * 3);
    unsigned int idx = 0;
    
    // first : top at (0,0,1)
    ico_vtx[idx*3+0] = 0;
    ico_vtx[idx*3+1] = 0;
    ico_vtx[idx*3+2] = 1;
    idx++;
    float elev = sinf( V_ANGLE );
    float xy_length = cosf( V_ANGLE );
    // first row at (x,y, sinf( V_ANGLE) )
    float angle = 0;
    for( unsigned int i = 0; i < 5; ++i) {
      ico_vtx[idx*3+0] = xy_length * cosf( angle );
      ico_vtx[idx*3+1] = xy_length * sinf( angle );
      ico_vtx[idx*3+2] = elev;
      idx++;
      angle += H_ANGLE;
    }
    // second row at (x,y, -sinf( V_ANGLE)), starting at angle=H_ANGLE/2
    angle = H_ANGLE / 2.0;
    for( unsigned int i = 0; i < 5; ++i) {
      ico_vtx[idx*3+0] = xy_length * cosf( angle );
      ico_vtx[idx*3+1] = xy_length * sinf( angle );
      ico_vtx[idx*3+2] = -elev;
      idx++;
      angle += H_ANGLE;
    }
    
    // last : bottom at (0,0,-1)
    ico_vtx[idx*3+0] = 0;
    ico_vtx[idx*3+1] = 0;
    ico_vtx[idx*3+2] = -1;
    idx++;

    if (verb) {
      for( unsigned int i = 0; i < 12; ++i) {
        std::cout << "vtx[" << i << "] = ";
        std::cout << std::setprecision(2) << ico_vtx[i*3+0] << ", ";
        std::cout << std::setprecision(2) << ico_vtx[i*3+1] << ", ";
        std::cout << std::setprecision(2) << ico_vtx[i*3+2];
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }
    return ico_vtx;
  }
  void build_faces_and_lines( bool verb=false )
  {
    // need vertices
    VTXvec ico_vtx = build_vertex( verb );

    // Hold x,y,z + r,g,b for every vertex of every face
    VTXvec faces_vtx;
    // Hold indexes of lines : i,i_next
    std::vector<GLuint> ico_idx(20 * 3);

    // Make the tour of the top row and add 4 faces vtx
    // And 6 lines
    /**   /\  1         /  
         /__\          /__ 
        2\  /\         \  /
          \/__\3        \/__
           \  /          \ 
            \/4           \
    */
    unsigned int idx = 0; 
    unsigned int idx_face=0;
    
    unsigned int i_top = 0;
    unsigned int i_bottom = 11;
    for( unsigned int i = 1; i <= 5; ++i) {
      unsigned int i_next = i+1;
      unsigned int i_below=i+5;
      unsigned int i_below_next=i_below+1;
      if (i==5) {
        i_next = 1;
        i_below_next = 6; 
      }

      // Top Face
      add_face( faces_vtx, ico_vtx, {i, i_next, i_top},
                _color_vec[idx_face+1], verb );
      idx_face++;
      // Side Face 2
      add_face( faces_vtx, ico_vtx, {i, i_below, i_next},
                _color_vec[idx_face+1], verb );
      idx_face++;
      // Side Face 3
      add_face( faces_vtx, ico_vtx, {i_next, i_below, i_below_next},
                _color_vec[idx_face+1], verb );
      idx_face++;
      // Bottom face
      add_face( faces_vtx, ico_vtx, {i_below, i_bottom, i_below_next},
                _color_vec[idx_face+1], verb );      
      idx_face++;

      // Lines (needs index of points in face_vtx, NOT ico_vtx)
      unsigned int idx_i = (i-1)*12;
      unsigned int idx_top = idx_i+2;
      unsigned int idx_next = idx_i+1;
      unsigned int idx_below = idx_i+4;
      unsigned int idx_below_next = idx_below+4;
      unsigned int idx_bottom = idx_below+6;
      // top to point
      ico_idx[idx++] = idx_top;
      ico_idx[idx++] = idx_i;
      // point to next in row
      ico_idx[idx++] = idx_i;
      ico_idx[idx++] = idx_next;
      // point to below
      ico_idx[idx++] = idx_i;
      ico_idx[idx++] = idx_below;
      // below to next in row
      ico_idx[idx++] = idx_below;
      ico_idx[idx++] = idx_next;
      // below to next below
      ico_idx[idx++] = idx_below;
      ico_idx[idx++] = idx_below_next;
      // below to bottom
      ico_idx[idx++] = idx_below;
      ico_idx[idx++] = idx_bottom;
    }

    _canvas.make_vao( faces_vtx, ico_idx,
                      _ico_faces_vao, _ico_faces_vbo, _ico_idxsides_vbo );
  }
  void add_face( VTXvec& dest, VTXvec& src,
                 const std::vector<unsigned int>& idx, 
                 const Color& col, bool verb=false )
  {
    if (verb) {
      std::cout << "add_face ";
      std::cout << " : (" << idx[0] << ", " << idx[1] << ", " << idx[2];
      std::cout << ")";
      std::cout << "  color = (" << col[0] << ", " << col[1] << ", " << col[2] << ")" << std::endl;
    }      
    // Vertices
    for( auto& i: idx) {
      
      dest.push_back( src[i*3+0] );
      dest.push_back( src[i*3+1] );
      dest.push_back( src[i*3+2] );

      // Color
      dest.push_back( col[0] );
      dest.push_back( col[1] );
      dest.push_back( col[2] );
    }
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

public:
  // *************************************************** IcoScreen::callback
  virtual void key_callback(int key, int scancode, int action, int mods)
  {
    //std::cout << "__ IcoScreen key=" << key << std::endl;
    GL3DCanvasScreen::key_callback( key, scancode, action, mods );
    
    // M
    if (key == GLFW_KEY_SEMICOLON && action == GLFW_PRESS) {
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
  // All Mouse Events are token from GL3DCanvasScreen
  
}; // IcoScreen

#endif // ICO_GLSCREEN_HPP