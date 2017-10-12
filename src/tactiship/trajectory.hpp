/* -*- coding: utf-8 -*- */

#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

/** 
 * Une suite de Pose = {glm::vec3 pos, glm::quat rot}.
 */
#include <gl_3dobject.hpp>
#include <gl_3dship.hpp>
#include <vector>                         // std::vector

// ********************************************************************* Types
typedef glm::vec3 Position;
typedef glm::quat Rotation;
typedef struct {
  Position _pos;
  Rotation _rot;
  bool     _display;
} Pose;
typedef std::vector<Pose> Trajectory;

// ******************************************************************** Global
#define RIBBON_WIDTH 0.25

// ***************************************************************************
// ************************************************************ GL3DTrajectory
// ***************************************************************************
class GL3DTrajectory : GL3DObject
{
public:
  // ************************************************ GL3DTrajectory::creation
  GL3DTrajectory( GL3DEnginePtr eng, const Trajectory& trajectory ) :
    GL3DObject( eng ),
    _traj(trajectory),
    _viewer_ship( eng )
  {
    // VBO pour le ruban : les bords comme x,y,z + rgb_line + rgb_face_front
    GLfloat ribbon_vtx[ 2 * (3+6) * _traj.size() ];
    unsigned int ribbon_idx = 0;
    for( auto& pt: _traj) {
      // Right tip
      Position rt(0,RIBBON_WIDTH,0);
      rt = pt._rot * rt;
      rt += pt._pos;
      ribbon_vtx[ribbon_idx++] = rt.x;
      ribbon_vtx[ribbon_idx++] = rt.y;
      ribbon_vtx[ribbon_idx++] = rt.z;
      std::cout << "rt = " <<  rt.x << "; " << rt.y << "; " << rt.z << std::endl;
      // Black
      ribbon_vtx[ribbon_idx++] = 0.0;
      ribbon_vtx[ribbon_idx++] = 0.0;
      ribbon_vtx[ribbon_idx++] = 0.0;
      // Green
      ribbon_vtx[ribbon_idx++] = 0.0;
      ribbon_vtx[ribbon_idx++] = 1.0;
      ribbon_vtx[ribbon_idx++] = 0.0;
      // Left tip
      Position lt(0,-RIBBON_WIDTH,0);
      lt = pt._rot * lt;
      lt += pt._pos;
      ribbon_vtx[ribbon_idx++] = lt.x;
      ribbon_vtx[ribbon_idx++] = lt.y;
      ribbon_vtx[ribbon_idx++] = lt.z;
      std::cout << "lt = " <<  lt.x << "; " << lt.y << "; " << lt.z << std::endl;
      // Black
      ribbon_vtx[ribbon_idx++] = 0.0;
      ribbon_vtx[ribbon_idx++] = 0.0;
      ribbon_vtx[ribbon_idx++] = 0.0;
      // Green
      ribbon_vtx[ribbon_idx++] = 0.0;
      ribbon_vtx[ribbon_idx++] = 1.0;
      ribbon_vtx[ribbon_idx++] = 0.0;
    }
     // Un VBO
    glGenBuffers(1, &_vbo_ribbon);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ribbon);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(ribbon_vtx), 
		 ribbon_vtx, GL_STATIC_DRAW);

    // Les index qui spécifieront les faces du ruban
    // 2 triangle pour chaque point après le premier
    GLushort ribbon_ets[ 2*3*(_traj.size()-1) ];
    unsigned idx = 0;
    for( unsigned int i = 0; i < _traj.size()-1; ++i) {
      ribbon_ets[idx++]=(i*2)+0;ribbon_ets[idx++]=(i*2)+1;ribbon_ets[idx++]=(i*2)+2;
      ribbon_ets[idx++]=(i*2)+2;ribbon_ets[idx++]=(i*2)+1;ribbon_ets[idx++]=(i*2)+3;
    }
    glGenBuffers(1, &_ibo_ribbon_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ribbon_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ribbon_ets),
  		 ribbon_ets, GL_STATIC_DRAW);

  }
  // ********************************************* GL3DTrajectory::destruction
  virtual ~GL3DTrajectory()
  {
    // Et les vbo, ibo
    glDeleteBuffers(1, &_vbo_ribbon);
  }
  // ************************************************** GL3DTrajectory::render
  void render( const glm::mat4& projection,
	       const glm::vec3& origin = {0,0,0},
	       const glm::quat& rotation = glm::quat(glm::vec3(0,0,0)),
	       const glm::vec3& scale = {1,1,1} ) const
  {
    // D'abord les bords du ruban
    glPushAttrib (GL_ENABLE_BIT);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);

    glUseProgram( _engine->gl_multicolor().program() );

    // Matrix (from GL3DObject)
    auto mvp = set_projection_mtx( projection,
				   origin, rotation, scale );
    glUniformMatrix4fv(_engine->gl_multicolor().uniform_mvp(), 1, GL_FALSE,
		       glm::value_ptr(mvp));
    
    // Fade
    glUniform1f( _engine->gl_multicolor().uniform_fade(), 1.0 );
    
    // Ruban
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_ribbon );
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_coord3d());
    /* Describe Vertices Array to OpenGL */
    // Bord gauche
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      9*sizeof(GLfloat), // stride -> car XYZ + 2*RGB
      0                  // offset of first element
  			  );	   
    // Color
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_v_color());
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_v_color(), // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      9*sizeof(GLfloat), // stride -> car XYZ + 2*RGB
      (GLvoid*) (6*sizeof(GLfloat)) // offset = 3 car commence par X,Y,Z+rgb_line
			  );
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ribbon_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    // std::cout << "Render ship avec " << size/3 << " triangles" << std::endl;
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    // Bord du Ruban
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_ribbon );
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_coord3d());
    /* Describe Vertices Array to OpenGL */
    // Bord gauche
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      2*9*sizeof(GLfloat), // stride -> car XYZ + 2*RGB
      0                  // offset of first element
  			  );	   
    // Color
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_v_color());
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_v_color(), // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      2*9*sizeof(GLfloat), // stride -> car XYZ + 2*RGB
      (GLvoid*) (3*sizeof(GLfloat)) // offset = 3 car commence par X,Y,Z
			  );
    // Push elements for drowing line
    glDrawArrays(GL_LINE_STRIP, 0, _traj.size() );

    // Bord droit
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_coord3d(), // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      2*9*sizeof(GLfloat), // stride -> car XYZ + RGB
      (GLvoid*) (9*sizeof(GLfloat)) // offset = 6 car deuxième pt
  			  );	   
    // Color
    glEnableVertexAttribArray(_engine->gl_multicolor().attribute_v_color());
    glVertexAttribPointer(
      _engine->gl_multicolor().attribute_v_color(), // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      2*9*sizeof(GLfloat), // stride -> car XYZ + 2RGB
      (GLvoid*) ((9+3)*sizeof(GLfloat)) // offset = 3 car commence par X,Y,Z
			  );
    // Push elements for drowing line
    glDrawArrays(GL_LINE_STRIP, 0, _traj.size() );
    
    glPopAttrib();
    
    // Un GL3DShip par point de la trajectoire
    for( auto& ship: _traj) {
      if( ship._display ) {
    	_viewer_ship.render( projection, ship._pos, ship._rot, {0.2,0.2,0.2} );
      }
    }
  }
  // *********************************************** GL3DTrajectory::attributs
private:
  /** Model */
  const Trajectory& _traj;
  /** Viewer */
  GL3DShip  _viewer_ship;
  /** Vertex Buffer Objects */
  GLuint _vbo_ribbon;
  GLuint _ibo_ribbon_elements;
};

#endif // TRAJECTORY_HPP

