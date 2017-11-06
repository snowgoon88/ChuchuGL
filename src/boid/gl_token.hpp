/* -*- coding: utf-8 -*- */

#ifndef GLTOKEN_HPP
#define GLTOKEN_HPP

#include <tactiship/gl_3dobject.hpp>

#include <list>

/** 
 * Display a list of tokens as a {DISK} with orientation (small triangle).
 * Can change the color of the Token and of the Triangle arrow.
 */
// ***************************************************************************
// ******************************************************************* GLToken
// ***************************************************************************
class GLToken : public GL3DObject
{
public:
  using TToken = struct pose_t_ {
    glm::vec3 origin;
    float orient;
    glm::vec3 token_color;
    glm::vec3 arrow_color;
  };
  using TListToken = std::list<TToken>;
public:
  // ******************************************************* GLToken::creation
  GLToken( GL3DEnginePtr eng, unsigned int disk_segment_nb = 8 ) :
    GL3DObject(eng)
  {
    // VBO with 3d coord for volume, but z=0.0.
    // 1 vertex by segment + center + arrow
	unsigned int nb_vertex = disk_segment_nb + 1 + 3;
    GLfloat disk_vtx[nb_vertex*3];
	for( unsigned int i = 0; i < disk_segment_nb; ++i) {
	  float x = cos( i * 2.f * M_PI / (float) disk_segment_nb );
	  float y = sin( i * 2.f * M_PI / (float) disk_segment_nb );
	  disk_vtx[i*3+0] = 0.5*x;
	  disk_vtx[i*3+1] = 0.5*y;
	  disk_vtx[i*3+2] = 0.0f;
	}
    // center
	disk_vtx[3 * disk_segment_nb + 0] = 0.f;
	disk_vtx[3 * disk_segment_nb + 1] = 0.f;
	disk_vtx[3 * disk_segment_nb + 2] = 0.f;
    // Arrow
	disk_vtx[3 * (disk_segment_nb+1) + 0] = 0.4f;
    disk_vtx[3 * (disk_segment_nb+1) + 1] = 0.f;
    disk_vtx[3 * (disk_segment_nb+1) + 2] = 0.01f;
	disk_vtx[3 * (disk_segment_nb+2) + 0] = 0.2f;
    disk_vtx[3 * (disk_segment_nb+2) + 1] = 0.2f;
    disk_vtx[3 * (disk_segment_nb+2) + 2] = 0.01f;
	disk_vtx[3 * (disk_segment_nb+3) + 0] = 0.2f;
    disk_vtx[3 * (disk_segment_nb+3) + 1] = -0.2f;
    disk_vtx[3 * (disk_segment_nb+3) + 2] = 0.01f;
    
    // VBO
    glGenBuffers(1, &_vbo_disk);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_disk);
    // Push points into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( disk_vtx ),
  		 disk_vtx, GL_STATIC_DRAW);

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Indexes for faces
    GLushort disk_ets[disk_segment_nb*3];
	for( unsigned int i = 0; i < disk_segment_nb; ++i) {
	  // up
	  disk_ets[i*3+0] = i;
	  disk_ets[i*3+1] = (i+1) % disk_segment_nb;
	  disk_ets[i*3+2] = disk_segment_nb;
	}
    glGenBuffers(1, &_ibo_disk_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_disk_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(disk_ets),
  		 disk_ets, GL_STATIC_DRAW);
    // Indexes for arrows
    GLushort arrow_ets[3];
    unsigned idx = 0;
    arrow_ets[0] = disk_segment_nb+1;
    arrow_ets[1] = disk_segment_nb+2;
    arrow_ets[2] = disk_segment_nb+3;
    glGenBuffers(1, &_ibo_arrow_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_arrow_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrow_ets),
  		 arrow_ets, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
  // **************************************************** GLToken::destruction
  virtual ~GLToken()
  {
    // vbo, ibo
    glDeleteBuffers(1, &_vbo_disk);
    glDeleteBuffers(1, &_ibo_disk_elements);
  }
  // ****************************************************** GLToken::rendering
  void render_list( const glm::mat4& projection,
                    const TListToken& list_token )
  {
    // for every token, change color and draw with origin, rotation, scale
    // uniform scale
    glm::vec3 scale{ 1,1,1 };
    // Token as Disc
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_disk );
    glEnableVertexAttribArray(_engine->gl_unicolor().attribute_coord3d());
    /* Describe Vertices Array to OpenGL */
    glVertexAttribPointer(
	  _engine->gl_unicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      3*sizeof(GLfloat), // stride -> car XYZ + RGB
      0                  // offset of first element
    );
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_disk_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    for (auto it = list_token.begin(); it != list_token.end(); ++it) {
      //set position
      auto mvp = set_projection_mtx( projection,
                                     it->origin,
                                     glm::angleAxis( it->orient,
                                                     glm::vec3{0,0,1} ),
                                     scale );
      glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
                         glm::value_ptr(mvp));
      // Color using OpenGL shaders
      glUniform3f( _engine->gl_unicolor().uniform_l_color(),
                   it->token_color.r,
                   it->token_color.g,
                   it->token_color.b );
      glUniform1f( _engine->gl_unicolor().uniform_fade(), 1.f );
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    }

    // And now the arrows...
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_arrow_elements);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    for (auto it = list_token.begin(); it != list_token.end(); ++it) {
      //set position
      auto mvp = set_projection_mtx( projection,
                                     it->origin,
                                     glm::angleAxis( it->orient,
                                                     glm::vec3{0,0,1} ),
                                     scale );
      glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
                         glm::value_ptr(mvp));
      // Color using OpenGL shaders
      glUniform3f( _engine->gl_unicolor().uniform_l_color(),
                   it->arrow_color.r,
                   it->arrow_color.g,
                   it->arrow_color.b );
      glUniform1f( _engine->gl_unicolor().uniform_fade(), 1.f );
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    }
  }
  void pre_render()
  {
    glPushAttrib (GL_ENABLE_BIT);
    glEnable (GL_CULL_FACE); //see only one face
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);

    glUseProgram( _engine->gl_unicolor().program() );
  }
  void post_render()
  {
    glPopAttrib();
  }
  // ******************************************************* GLToken::attibuts
private:
  /** Vertex Buffer Objects */
  GLuint _vbo_disk;
  GLuint _ibo_disk_elements, _ibo_arrow_elements;
};


#endif // GLTOKEN_HPP
