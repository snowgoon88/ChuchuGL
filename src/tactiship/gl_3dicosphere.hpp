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
public:
  // ************************************************* GL3DIcosphere::creation
  GL3DIcosphere( GL3DEnginePtr eng) : 
    GL3DObject(eng)
  {
    set_color( {1.0f, 1.0f, 1.0f}, 1.f );
    // ****** VERTEX *******************
    // build points: 12 * xyz
    std::vector<GLfloat> ico_vtx(12 * 3);
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

    for( unsigned int i = 0; i < 12; ++i) {
      std::cout << "vtx[" << i << "] = ";
      std::cout << std::setprecision(2) << ico_vtx[i*3+0] << ", ";
      std::cout << std::setprecision(2) << ico_vtx[i*3+1] << ", ";
      std::cout << std::setprecision(2) << ico_vtx[i*3+2];
      std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // Un VBO
    glGenBuffers(1, &_vbo_ico);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ico);
    // push vyx into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 3 * idx,
  		 ico_vtx.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    std::cout << "  pushed " << idx << " vtx into _vbo_ico" << std::endl;

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
    idx = 0;
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
    std::cout << "  pushed " << idx << " idx into _vbo_ico_lines" << std::endl;
  }
  // ********************************************** GL3DIcosphere::destruction
  virtual ~GL3DIcosphere()
  {
    // Et les vbo, ibo
    glDeleteBuffers(1, &_vbo_ico);
    glDeleteBuffers(1, &_ibo_ico_lines);
  }
  // *************************************************** GL3DIcosphere::render
  void render( const glm::mat4& projection,
	       const glm::vec3& origin = {0,0,0},
	       const glm::quat& rotation = glm::quat(glm::vec3(0,0,0)),
	       const glm::vec3& scale = {1,1,1} ) const
  {
    glPushAttrib (GL_ENABLE_BIT);
    glEnable (GL_LINE_SMOOTH);

    glUseProgram( _engine->gl_unicolor().program() );

    // GLObject -> set projection matrix
    auto mvp = set_projection_mtx( projection,
				   origin, rotation, scale );
    glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
		       glm::value_ptr(mvp));


    // Color & Fade
    set_color_to_gl();

    // IcoLines
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_ico );
    glEnableVertexAttribArray(_engine->gl_unicolor().attribute_coord3d());
    /* Describe Vertices Array to OpenGL */
    glVertexAttribPointer(
      _engine->gl_unicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      3*sizeof(GLfloat), // stride -> car XYZ
      0                  // offset of first element
  			  );
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ico_lines);
     // Push elements for drowing line
    glDrawElements(GL_LINES, 2*30, GL_UNSIGNED_SHORT, 0);
    
    glPopAttrib();
  }
  // ************************************************ GL3DIcosphere::attributs
private:
  /** Vertex Buffer Objects */
  GLuint _vbo_ico;
  GLuint _ibo_ico_lines;
  
}; // GL3DIcosphere

#endif // GL3DICOSPHERE_HPP
