/* -*- coding: utf-8 -*- */

#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

/** 
 * Une suite de Pose = {glm::vec3 pos, glm::quat rot}.
 */

#include <gl_3dship.hpp>
#include <vector>                         // std::vector

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>           // glm::quat 
// #include <glm/gtx/quaternion.hpp>           // glm::quat toMat4

// ********************************************************************* Types
typedef glm::vec3 Position;
typedef glm::quat Rotation;
typedef struct {
  Position _pos;
  Rotation _rot;
} Pose;
typedef std::vector<Pose> Trajectory;

// ***************************************************************************
// ************************************************************ GL3DTrajectory
// ***************************************************************************
class GL3DTrajectory
{
public:
  // ************************************************ GL3DTrajectory::creation
  GL3DTrajectory( const Trajectory& trajectory ) :
    _traj(trajectory),
    _viewer_ship()
  {
  };
  // ************************************************** GL3DTrajectory::render
  void render( const glm::mat4& projection,
  	       const glm::vec3& origin = {0,0,0},
	       const glm::quat& rotation = {0,0,0,1}) const
  {
    // Un GL3DShip par point de la trajectoire
    for( auto& ship: _traj) {
      _viewer_ship.render( projection, ship._pos, ship._rot );
    }
  }
  // *********************************************** GL3DTrajectory::attributs
private:
  /** Model */
  const Trajectory& _traj;
  /** Viewer */
  GL3DShip  _viewer_ship;
};

#endif // TRAJECTORY_HPP
