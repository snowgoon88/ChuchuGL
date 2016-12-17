/* -*- coding: utf-8 -*- */

#ifndef GL_3DFRAME_HPP
#define GL_3DFRAME_HPP

/** 
 * Repère 3D avec 3 GL3DVect.
 */
#include <gl_3dvect.hpp>
#include <gl_3dtext_shaders.hpp>

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
    _vect_viewer( eng ),
	_gl_text( eng.gl_text() )
  {
  };
  // ******************************************************* GL3DFrame::render
  void render( const glm::mat4& projection )
  {
	
    // 1ère flèche normale
    _vect_viewer.render( projection );
	_gl_text.pre_render( projection );
	_gl_text.set_color( {1.f, 0.f, 0.f, 1.f} );
	_gl_text.render( "X", 1.1f, 0.f);
	_gl_text.post_render();
    // 2ème, green avec rotation
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),
				     (float) M_PI/2.f,
				     glm::vec3(0,0,1));
    glm::mat4 vpm = projection * rotation;
    _vect_viewer.render( vpm, {0,1,0} /* green */ );
	_gl_text.pre_render( vpm );
	_gl_text.set_color( {0.f, 1.f, 0.f, 1.f} );
	_gl_text.render( "Y", 1.1f, 0.f);
	_gl_text.post_render();

    // 3ème, blue avec rotation
    rotation = glm::rotate( glm::mat4(1.0f), //rotation,
			    -(float) M_PI/2.f,
			    glm::vec3(0,1,0));
    vpm = projection * rotation;
    _vect_viewer.render( vpm, {0,0,1} /* blue */ );
	_gl_text.pre_render( vpm );
	_gl_text.set_color( {0.f, 0.f, 1.f, 1.f} );
	_gl_text.render( "Z", 1.1f, 0.f);
	_gl_text.post_render();
  };
  // **************************************************** GL3DFrame::attributs
private:
  //GL3DUnicolor _eng;
  GL3DVect _vect_viewer;
  GL3DTextShaders& _gl_text;
};
#endif // GL_3DFRAME_HPP
