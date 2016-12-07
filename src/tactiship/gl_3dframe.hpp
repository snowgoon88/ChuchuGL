/* -*- coding: utf-8 -*- */

#ifndef GL_3DFRAME_HPP
#define GL_3DFRAME_HPP

/** 
 * Repère 3D avec 3 GL3DVect.
 */
#include <gl_3dvect.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ***************************************************************************
// ***************************************************************** GL3DFrame
// ***************************************************************************
class GL3DFrame
{
public:
  // ***************************************************** GL3DFrame::creation
  GL3DFrame( GL3DEngine& eng ) :
	//_eng("src/shaders/line3d"),
    _vect_viewer( eng )
  {
  };
  // ******************************************************* GL3DFrame::render
  void render( const glm::mat4& projection )
  {
    // 1ère flèche normale
    _vect_viewer.render( projection );
    // 2ème, green avec rotation
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),
				     (float) M_PI/2.f,
				     glm::vec3(0,0,1));
    glm::mat4 vpm = projection * rotation;
    _vect_viewer.render( vpm, {0,1,0} /* green */ );
    // 3ème, blue avec rotation
    rotation = glm::rotate( glm::mat4(1.0f), //rotation,
			    -(float) M_PI/2.f,
			    glm::vec3(0,1,0));
    vpm = projection * rotation;
    _vect_viewer.render( vpm, {0,0,1} /* blue */ );
  };
  // **************************************************** GL3DFrame::attributs
private:
  //GL3DUnicolor _eng;
  GL3DVect _vect_viewer;
};
#endif // GL_3DFRAME_HPP
