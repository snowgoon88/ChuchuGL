/* -*- coding: utf-8 -*- */

#ifndef GL_3DGRID_HPP
#define GL_3DGRID_HPP

/** 
 * A "horizontal grid" with a "gap" between lines.
 */
#include <gl_3dobject.hpp>

// ***************************************************************************
// ****************************************************************** GL3DGrid
// ***************************************************************************
class GL3DGrid : public GL3DObject
{
public:
  // ****************************************************** GL3DGrid::creation
  GL3DGrid( GL3DEnginePtr eng,
			float xmin, float xmax, float xgap,
			float ymin, float ymax, float ygap) : GL3DObject(eng)
  {
	// VBO for grid
	// nb of lines along Ox and Oy
	unsigned int xnb = static_cast<unsigned int>(trunc((xmax - xmin) / xgap));
	unsigned int ynb = static_cast<unsigned int>(trunc((ymax - ymin) / ygap));

	GLfloat line_vtx[(xnb+1+ynb+1)*6];
    for( unsigned int idx = 0; idx < xnb+1; ++idx) {
      line_vtx[idx*6] = xmin + xgap * (float) idx;
      line_vtx[idx*6+1] = ymin;
	  line_vtx[idx*6+2] = 0.f;
	  
      line_vtx[idx*6+3] = xmin + xgap * (float) idx;
      line_vtx[idx*6+4] = ymax;
	  line_vtx[idx*6+5] = 0.f;
    }
    for( unsigned int idy = 0; idy < ynb+1; ++idy) {
      line_vtx[(xnb+1)*6+idy*6] = xmin;
	  line_vtx[(xnb+1)*6+idy*6+1] = ymin + ygap * (float) idy;
	  line_vtx[(xnb+1)*6+idy*6+2] = 0.f;

	  line_vtx[(xnb+1)*6+idy*6+3] = xmax;
	  line_vtx[(xnb+1)*6+idy*6+4] = ymin + ygap * (float) idy;
	  line_vtx[(xnb+1)*6+idy*6+5] = 0.f;
	}
    _vbo_lines_size = 2*(xnb+1 + ynb+1);
    // Un VBO
    glGenBuffers(1, &_vbo_lines);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_lines);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vtx), 
				 line_vtx, GL_STATIC_DRAW);
  }
  // *************************************************** GL3DGrid::destruction
  virtual ~GL3DGrid()
  {
	// VBO
	glDeleteBuffers(1, &_vbo_lines);
  }
  // ******************************************************** GL3DGrid::render
  void render( const glm::mat4& projection,
			   const glm::vec3& fg_color = {1,0,0} ) const
  {
	glUseProgram( _engine->gl_unicolor().program() );
	glUniformMatrix4fv(_engine->gl_unicolor().uniform_mvp(), 1, GL_FALSE,
					   glm::value_ptr(projection));
    // Color
	glUniform3f( _engine->gl_unicolor().uniform_l_color(),
				 fg_color.r, fg_color.g, fg_color.b );
	glUniform1f( _engine->gl_unicolor().uniform_fade(), 1.0 );

	glEnableVertexAttribArray( _engine->gl_unicolor().attribute_coord3d() );
    /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glBindBuffer( GL_ARRAY_BUFFER, _vbo_lines );
    glVertexAttribPointer(
	  _engine->gl_unicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      0,                 // stride
      0                  // offset of first element
	);
	/* Push each element in buffer_vertices to the vertex shader */
    glDrawArrays(GL_LINES, 0, _vbo_lines_size);
  }
  // ***************************************************** GL3DGrid::attributs
private:
  /** Vertex Buffer Object for lines */
  GLuint _vbo_lines;
  unsigned int _vbo_lines_size;
};

#endif // GL_3DGRID_HPP
