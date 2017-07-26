/* -*- coding: utf-8 -*- */

#ifndef GL_3DOBJECT_HPP
#define GL_3DOBJECT_HPP

/** 
 * Interface for 3D Object using OpenGL.
 *
 * Can render as only a local frame (3 vect X,Y,Z).
 */
#include <gl_3dengine.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>           // glm::quat 
#include <glm/gtx/quaternion.hpp>           // glm::quat toMat4

// ***************************************************************************
// **************************************************************** GL3DObject
// ***************************************************************************
class GL3DObject
{
public:
  using TColRGB = glm::vec3;
public:
  // **************************************************** GL3DObject::creation
  GL3DObject( GL3DEnginePtr engine ) :
	_engine( engine )
  {}
  /** copy creation */
  GL3DObject( const GL3DObject& obj ) :
	_engine( obj._engine )
  {}
  /** assignment creation */
  GL3DObject& operator=( const GL3DObject& obj )
  {
    if (this != &obj) { // protect against invalid self-assignment
	  _engine = obj._engine;
    }
    return *this;
  }
  // ************************************************* GL3DObject::destruction
  virtual ~GL3DObject() {}
  // ****************************************************** GL3DObject::render
  virtual void render( const glm::mat4& projection,
					   const glm::vec3& origin = {0,0,0},
					   const glm::quat& rotation = glm::quat(glm::vec3(0,0,0)),
					   const glm::vec3& scale = {1,1,1} ) const
  {
	std::cout << "GL3DObject::render() should ne be called" << std::endl;
  }
  /** Set up a MVP matrix using origin,rotation and scale */
  glm::mat4 set_projection_mtx( const glm::mat4& projection,
					   const glm::vec3& origin = {0,0,0},
					   const glm::quat& rotation = glm::quat(glm::vec3(0,0,0)),
					   const glm::vec3& scale = {1,1,1} ) const
  {
	// Scale
    glm::mat4 scale_mtx = glm::scale( glm::mat4(1.0f),
				      scale );
    // Rotation
    glm::mat4 rotation_mtx = glm::toMat4( rotation );

    // Translation
    glm::mat4 translation_mtx = glm::translate(  glm::mat4(1.0f),
						 origin );
    glm::mat4 mvp = projection * translation_mtx * rotation_mtx * scale_mtx;

	return mvp;
  }
  // ********************************************************* GLObject::color
  void set_color( const TColRGB& color, GLfloat fade = 1.f )
  {
	_color = color;
	_fade = fade;
  }
  void set_color_to_gl() const
  {
	// Color using OpenGL shaders
	glUniform3f( _engine->gl_unicolor().uniform_l_color(),
				 _color.r, _color.g, _color.b );
    glUniform1f( _engine->gl_unicolor().uniform_fade(), _fade );
  }
  // ***************************************************** GLObject::attributs
public:
  GL3DEnginePtr _engine;
  TColRGB _color;
  GLfloat _fade;
};


#endif // GL_3DOBJECT_HPP
