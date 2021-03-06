/* -*- coding: utf-8 -*- */

#ifndef GL_3DVECT_HPP
#define GL_3DVECT_HPP

/** 
 * GL3DObject that draw a vector as an arrow.
 */
#include <gl_3dobject.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>

// ******************************************************************** Global
#define LHEAD 0.1f
#define WHEAD 0.02f

// ***************************************************************************
// ****************************************************************** GL3DVect
// ***************************************************************************
class GL3DVect : public GL3DObject
{
public:
  // ****************************************************** GL3DVect::creation
  GL3DVect( GL3DEnginePtr eng ) : GL3DObject(eng)
  {
    // VBO pour le corp
    GLfloat tail_vtx[] = {0, 0, 0, 1.0,0,0};
    // Un VBO
    glGenBuffers(1, &_vbo_tail);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tail);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( tail_vtx ),
		 tail_vtx, GL_STATIC_DRAW);
    _vbo_tail_size = 2;
    // VBO pour les faces de la tête
    // On va utiliser un triangle fan : 3 pts pour premier triangle
    // puis chaque nouveau pt = 1 triangle => 6 pts : 6*3 float;
    GLfloat head_vtx[] = {
      0, 0, 0,       -LHEAD, 0, WHEAD,      -LHEAD, -WHEAD, 0,
      -LHEAD, 0, -WHEAD,
      -LHEAD, WHEAD, 0,
      -LHEAD, 0, WHEAD };
    _vbo_head_size = 6;
    // Un VBO
    glGenBuffers(1, &_vbo_head);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_head);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( head_vtx ),
		 head_vtx, GL_STATIC_DRAW);
  }
  // *************************************************** GL3DVect::destruction
  virtual ~GL3DVect()
  {
    // Et les vbo
    glDeleteBuffers(1, &_vbo_head);
    glDeleteBuffers(1, &_vbo_tail);
  }
  // ******************************************************** GL3DVect::render
  virtual void render( const glm::mat4& projection,
					   const glm::vec3& fg_color = {1,0,0},
					   const glm::vec3& vector = {1,0,0},
					   const glm::vec3& origin = {0,0,0}) const
  {
	glUseProgram( _engine->gl_unicolor().program() );
	glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
					   glm::value_ptr(projection));
    
    // Color of arrow
	glUniform3f( _engine->gl_unicolor().uniform_l_color(),
				 fg_color.r, fg_color.g, fg_color.b );
	glUniform1f( _engine->gl_unicolor().uniform_fade(), 1.0 );
	
	glEnableVertexAttribArray( _engine->gl_unicolor().attribute_coord3d() );
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    // TAIL *****************************************
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_tail );
    glVertexAttribPointer(
	  _engine->gl_unicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
      /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_LINES, 0, _vbo_tail_size);
    // HEAD *****************************************
    // Translation
    glm::mat4 translation = glm::translate(  glm::mat4(1.0f),
					     glm::vec3( 1.0f,
							0.f,
							0.f));
	// Projection-View
    glm::mat4 vp = projection * translation;
    glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
					   glm::value_ptr(vp));
    // Dessiner la tête
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_head );
	glEnableVertexAttribArray( _engine->gl_unicolor().attribute_coord3d() );
  /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
    glVertexAttribPointer(
	  _engine->gl_unicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
    /* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLE_FAN, 0, _vbo_head_size);
    
  };
  // ***************************************************** GL3DVect::attributs
private:
  /** Vertex Buffer Object pour lines */
  GLuint _vbo_head, _vbo_tail;
  unsigned int _vbo_head_size, _vbo_tail_size;
};
#endif // GL_3DVECT_HPP
