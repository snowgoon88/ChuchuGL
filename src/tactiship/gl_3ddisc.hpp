/* -*- coding: utf-8 -*- */

#ifndef GL3DDISC_HPP
#define GL3DDIC_HPP

/** 
 * Display a cylinder, axe 0y, radius=1, height=1.
 * At pos={0,0,0}, Can be scaled.
 *
 * unsigned int nb_segments in a circonference.
 */

#include <gl_3dengine.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>           // glm::quat 
#include <glm/gtx/quaternion.hpp>           // glm::quat toMat4

// ***************************************************************************
// ****************************************************************** GL3DDisc
// ***************************************************************************
class GL3DDisc
{
public:
  // ****************************************************** GL3DDisc::creation
  GL3DDisc( GL3DEngine& eng, unsigned int nb_segment ) : _eng(eng)
  {
    // VBO with 3d coord for volume.
	unsigned int nb_vertex = 2 * nb_segment + 2;
    GLfloat disk_vtx[nb_vertex*3];
	for( unsigned int i = 0; i < nb_segment; ++i) {
	  float x = cos( i * 2.f * M_PI / (float) nb_segment );
	  float y = sin( i * 2.f * M_PI / (float) nb_segment );
	  disk_vtx[i*3+0] = x;
	  disk_vtx[i*3+1] = y;
	  disk_vtx[i*3+2] = 0.5f;

	  disk_vtx[i*3+0+3*nb_segment] = x;
	  disk_vtx[i*3+1+3*nb_segment] = y;
	  disk_vtx[i*3+2+3*nb_segment] = -0.5f;
	}
	disk_vtx[6 * nb_segment + 0] = 0.f;
	disk_vtx[6 * nb_segment + 1] = 0.f;
	disk_vtx[6 * nb_segment + 2] = 0.5f;
	disk_vtx[6 * nb_segment + 3] = 0.f;
	disk_vtx[6 * nb_segment + 4] = 0.f;
	disk_vtx[6 * nb_segment + 5] = -0.5f;

	for( unsigned int i = 0; i < nb_vertex; ++i) {
	  std::cout << i << ":" << disk_vtx[i*3+0] << ", " << disk_vtx[i*3+1] << ", " << disk_vtx[i*3+2] << std::endl;
	}

    // Un VBO
    glGenBuffers(1, &_vbo_disk);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_disk);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( disk_vtx ),
  		 disk_vtx, GL_STATIC_DRAW);
    // Qu'on peut rendre inactif avec
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Indexes for faces
    GLushort disk_ets[4*nb_segment*3];
	for( unsigned int i = 0; i < nb_segment; ++i) {
	  // lateral face
	  disk_ets[i*12+0] = i;
	  disk_ets[i*12+1] = i+nb_segment;
	  disk_ets[i*12+2] = (i+1) % nb_segment;
	  // lateral face
	  disk_ets[i*12+3] = i+nb_segment;
	  disk_ets[i*12+4] = (i+1) % nb_segment + nb_segment;
	  disk_ets[i*12+5] = (i+1) % nb_segment;
	  // up
	  disk_ets[i*12+6] = i;
	  disk_ets[i*12+7] = (i+1) % nb_segment;
	  disk_ets[i*12+8] = 2*nb_segment;
	  // down
	  disk_ets[i*12+9] = (i+1) % nb_segment + nb_segment;
	  disk_ets[i*12+10] = i+nb_segment;
	  disk_ets[i*12+11] = 2*nb_segment+1;
	}
    glGenBuffers(1, &_ibo_disk_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_disk_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(disk_ets),
  		 disk_ets, GL_STATIC_DRAW);
    // Optionel ?
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for( unsigned int i = 0; i < nb_segment; ++i) {
	  std::cout << disk_ets[i*12+0] << " - " << disk_ets[i*12+1] << " - " << disk_ets[i*12+2] << std::endl;
	  std::cout << disk_ets[i*12+3] << " - " << disk_ets[i*12+4] << " - " << disk_ets[i*12+5] << std::endl;
	  std::cout << disk_ets[i*12+6] << " - " << disk_ets[i*12+7] << " - " << disk_ets[i*12+8] << std::endl;
	  std::cout << disk_ets[i*12+9] << " - " << disk_ets[i*12+10] << " - " << disk_ets[i*12+11] << std::endl;
	}

	
  }
  // *************************************************** GL3DDisc::destruction
  virtual ~GL3DDisc()
  {
    // Et les vbo, ibo
    glDeleteBuffers(1, &_vbo_disk);
    glDeleteBuffers(1, &_ibo_disk_elements);
  };
  // ******************************************************** GL3DDisc::render
  void render( const glm::mat4& projection,
			   const glm::vec3& origin = {0,0,0},
			   const glm::vec3& scale = {1,1,1},
			   const glm::vec3& fg_color = {1,0,0},
			   const glm::quat& rotation = {0,0,0,1} ) const
  {
    glPushAttrib (GL_ENABLE_BIT);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);

    glUseProgram( _eng.gl_unicolor().program() );
    // Color of disc
	glUniform3f( _eng.gl_unicolor().uniform_l_color(),
				 fg_color.r, fg_color.g, fg_color.b );

    // Scale
    glm::mat4 scale_mtx = glm::scale( glm::mat4(1.0f),
				      scale );
    // Rotation
    glm::mat4 rotation_mtx = glm::toMat4( rotation );

    // Translation
    glm::mat4 translation_mtx = glm::translate(  glm::mat4(1.0f),
						 origin );
    glm::mat4 mvp = projection * translation_mtx * rotation_mtx * scale_mtx;
    glUniformMatrix4fv(_eng.gl_unicolor().uniform_mvp(), 1, GL_FALSE,
     		       glm::value_ptr(mvp));

    // Disc
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_disk );
    glEnableVertexAttribArray(_eng.gl_unicolor().attribute_coord3d());
    /* Describe Vertices Array to OpenGL */
    glVertexAttribPointer(
	  _eng.gl_unicolor().attribute_coord3d(), // attribute
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
    // std::cout << "Render ship avec " << size/3 << " triangles" << std::endl;
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	

    //glDisableVertexAttribArray(_attribute_coord3d);
    //glDisableVertexAttribArray(_attribute_v_color);

    glPopAttrib();
  };
  // ***************************************************** GL3DDisc::attributs
private:
 /** Graphic Engine for lines/triangles */
 GL3DEngine& _eng;
  /** Vertex Buffer Objects */
  GLuint _vbo_disk;
  GLuint _ibo_disk_elements;
  
};
#endif // GL3DSHIP_HPP
