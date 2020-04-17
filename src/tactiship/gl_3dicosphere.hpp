/* -*- coding: utf-8 -*- */

#ifndef GL3DICOSPHERE_HPP
#define GL3DICOSPHERE_HPP

/** 
 * TODO 
 */
#include <gl_3dobject.hpp>
#include <vector>

class GL3DIcosphere : public GL3DObject
{
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
public:
  // ************************************************* GL3DIcosphere::creation
  GL3DIcosphere( GL3DEnginePtr eng) : 
    GL3DObject(eng)
  {
    set_color( {1.0f, 1.0f, 1.0f}, 1.f );
    //build_lines( false );
    build_faces( true );
  }
  // ********************************************** GL3DIcosphere::destruction
  virtual ~GL3DIcosphere()
  {
    // Et les vbo, ibo
    glDeleteBuffers(1, &_vbo_faces);
    // glDeleteBuffers(1, &_vbo_ico);
    // glDeleteBuffers(1, &_ibo_ico_lines);
  }
  // *************************************************** GL3DIcosphere::render
  void render( const glm::mat4& projection,
	       const glm::vec3& origin = {0,0,0},
	       const glm::quat& rotation = glm::quat(glm::vec3(0,0,0)),
	       const glm::vec3& scale = {1,1,1} ) const
  {
    glPushAttrib (GL_ENABLE_BIT);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);

    glUseProgram( _engine->gl_multicolor().program() );

    // GLObject -> set projection matrix
    auto mvp = set_projection_mtx( projection,
				   origin, rotation, scale );
    glUniformMatrix4fv(_engine->gl_multicolor().uniform_mvp(), 1, GL_FALSE,
		       glm::value_ptr(mvp));
    // Fade
    glUniform1f( _engine->gl_multicolor().uniform_fade(), 1.0 );

    // Color & Fade
    //set_color_to_gl();
    
    // // IcoLines
    // glBindBuffer( GL_ARRAY_BUFFER, _vbo_ico );
    // glEnableVertexAttribArray(_engine->gl_unicolor().attribute_coord3d());
    // /* Describe Vertices Array to OpenGL */
    // glVertexAttribPointer(
    //   _engine->gl_unicolor().attribute_coord3d(), // attribute
    //   3,                 // number of elements per vertex, here (x,y,z)
    //   GL_FLOAT,          // the type of each element
    //   GL_FALSE,          // take our values as-is
    //   3*sizeof(GLfloat), // stride -> car XYZ
    //   0                  // offset of first element
    //     		  );
    // /* Push each element in buffer_vertices to the vertex shader
    //  * according to index */
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ico_lines);
    //  // Push elements for drowing line
    // glDrawElements(GL_LINES, 2*30, GL_UNSIGNED_SHORT, 0);

    // IcoFaces
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_faces );
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_coord3d());
    /* Describe Vertices Array to OpenGL */
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      6*sizeof(GLfloat), // stride -> because XYZ + RGB
      0                  // offset of first element
        		  );
    
    // Color
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_v_color());
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_v_color(), // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      6*sizeof(GLfloat), // stride -> car XYZ + RGB
      (GLvoid*) (3*sizeof(GLfloat)) // offset = 3 as starts with X,Y,Z
			  );

    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLES, 0, 3*20);
        
    glPopAttrib();
  }
  // ************************************************ GL3DIcosphere::attributs
private:
  /** Vertex Buffer Objects */
  GLuint _vbo_ico, _vbo_faces;
  GLuint _ibo_ico_lines;

  // ********************************************* GL3DIcosphere::build_vertex
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
  /** Build Lines
   */
  void build_lines( bool verb=false )
  {
    // ****** VERTEX *******************
    // build points: 12 * xyz
    VTXvec ico_vtx = build_vertex( false );
    // Un VBO
    glGenBuffers(1, &_vbo_ico);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ico);
    // push vyx into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * ico_vtx.size(),
  		 ico_vtx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (verb) {
      std::cout << "  pushed " << ico_vtx.size() << " vtx into _vbo_ico" << std::endl;
    }

    // ****** INDEX ARRAY  for Lines ***
    std::vector<GLushort> ico_idx(20 * 3);
    // Make the tour of the top row and add 6 lines per vtx
    /**   /
         /__
         \  /
          \/__
           \
            \
    */
    unsigned int idx = 0;       
    for( unsigned int i = 1; i <= 5; ++i) {
      // top to point
      ico_idx[idx++] = 0;
      ico_idx[idx++] = i;


      unsigned int i_next=i+1;
      unsigned int i_below=i+5;
      unsigned int i_below_next=i_below+1;
      if (i==5) {
        i_next = 1;
        i_below_next = 6; 
      }
      
      // point to next in row
      ico_idx[idx++] = i;
      ico_idx[idx++] = i_next;
      // point to below
      ico_idx[idx++] = i;
      ico_idx[idx++] = i_below;
      // below to next in row
      ico_idx[idx++] = i_below;
      ico_idx[idx++] = i_next;
      // below to next below
      ico_idx[idx++] = i_below;
      ico_idx[idx++] = i_below_next;
      // below to bottom
      ico_idx[idx++] = i_below;
      ico_idx[idx++] = 11;
    }
    for( unsigned int i = 0; i < 30; ++i) {
      std::cout << "idx[" << i*2+0 << "] = " << ico_idx[i*2+0];
      std::cout << " -> ";
      std::cout << "idx[" << i*2+1 << "] = " << ico_idx[i*2+1];
      std::cout << std::endl;
    }
    std::cout << std::endl;

    
    glGenBuffers(1, &_ibo_ico_lines);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ico_lines);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * idx,
  		 ico_idx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (verb) {
      std::cout << "  pushed " << idx << " idx into _vbo_ico_lines" << std::endl;
    }
  }
  /** Build Faces as independant triangles. Each one has a different
   * color
   */
  void build_faces( bool verb=false )
  {
    // need vertices
    VTXvec ico_vtx = build_vertex( verb );

    VTXvec faces_vtx;
    // Make the tour of the top row and add 4 faces vtx
    /**   /\  1
         /__\
        2\  /\
          \/__\3
           \  /
            \/4
    */
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
    }

    // Build a VBO
    // Un VBO
    glGenBuffers(1, &_vbo_faces);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_faces);
    // push xyz+rgb into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * faces_vtx.size(),
  		 faces_vtx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (verb) {
      std::cout << "  pushed " <<  faces_vtx.size() << " vtx into _vbo_faces" << std::endl;
    }
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
}; // GL3DIcosphere

#endif // GL3DICOSPHERE_HPP
