/* -*- coding: utf-8 -*- */

#ifndef GL3DRECT_HPP
#define GL3DRECT_HPP

/** 
 * Display a Rectangle, plan 0xy, centered at 0,0,0. Size 1.
 */
#include <gl_3dobject.hpp>

// ***************************************************************************
// ****************************************************************** GL3DRect
// ***************************************************************************
class GL3DRect : public GL3DObject
{
public:
  // ****************************************************** GL3DRect::creation
  GL3DRect( GL3DEnginePtr eng) :
	GL3DObject(eng)
  {
	// VBO vertices for 3d coord
	GLfloat rect_vtx[] = {
	  0.5f, -0.5f, 0.0f,
	  0.5f, 0.5f, 0.0f,
	  -0.5f, 0.5f, 0.0f,
	  -0.5f, 0.5f, 0.0f,
	  -0.5f, -0.5f, 0.0f,
	  0.5f, -0.5f, 0.0f };
	_rect_vtx_size = 6;
	// Un VBO
    glGenBuffers(1, &_vbo_rect);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_rect);
    // Push points into VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( rect_vtx ),
  		 rect_vtx, GL_STATIC_DRAW);
  }
  // *************************************************** GL3DRect::destruction
  virtual ~GL3DRect()
  {
    // Et les vbo, ibo
    glDeleteBuffers(1, &_vbo_rect);
  }
  // ******************************************************** GL3DRect::render
  virtual void render( const glm::mat4& projection,
					   const glm::vec3& origin = {0,0,0},
					   const glm::quat& rotation = glm::quat(glm::vec3(0,0,0)),
					   const glm::vec3& scale = {1,1,1} ) const
  {
	glUseProgram( _engine->gl_unicolor().program() );

	// Color and Fade (from GL3DObject)
	set_color_to_gl();
	// Matrix (from GL3DObject)
	auto mvp = set_projection_mtx( projection,
								   origin, rotation, scale );
	glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
					   glm::value_ptr(mvp));
	

	// Draw
	glBindBuffer( GL_ARRAY_BUFFER, _vbo_rect );
    glVertexAttribPointer(
	  _engine->gl_unicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
			  );
	/* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_TRIANGLES, 0, _rect_vtx_size);
  }
  // ***************************************************** GL3DRect::attributs
private:
  /** Vertex Buffer Object for Rectangle */
  GLuint _vbo_rect;
  unsigned int _rect_vtx_size;
};

#endif // GL3DRECT_HPP
