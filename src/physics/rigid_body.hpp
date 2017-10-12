/* -*- coding: utf-8 -*- */

#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

/** 
 * Implementation of a 3D rigid body.
 * - mass
 * - pos : position of center of mass, vector
 * - vel :velocity vector
 * - acc :acceleration vector
 *
 * - rot : rotation as quaternion
 *
 * At every update, a Force can be applied to update its pos,vel,acc
 * - force : applied force for the given update interval
 */
#include <string>                     // std::string
#include <sstream>                    // std::stringdtream
#include <memory>                        // std::*_ptr

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>     // definition
#include <glm/gtx/quaternion.hpp>     // rotation of vec3
// ***************************************************************************
// ***************************************************************** RigidBody
// ***************************************************************************
namespace physics {
  using TNumber = double;
  using TMass = TNumber;
  using TVec3 = glm::vec3;
  using TRot = glm::quat;
  class RigidBody;
  using RigidBodyPtr = std::shared_ptr<RigidBody>;
class RigidBody
{
public:
  // ***************************************************** RigidBody::creation
  RigidBody( const TMass mass = 1 ) :
	_mass(mass), _pos( TVec3(0,0,0)),
	_vel( TVec3(0,0,0)), _acc( TVec3(0,0,0)),
	_rot( glm::quat(glm::vec3(0,0,0)) )
  {
  }
  /** copy creation */
  RigidBody( const RigidBody& rb ) :
	_mass( rb._mass), _pos( rb._pos),
	_vel( rb._vel), _acc( rb._acc ),
	_rot( rb._rot )
  {}
  /** assignment creation */
  RigidBody& operator=( const RigidBody& rb )
  {
    if (this != &rb) { // protect against invalid self-assignment
	  _mass = rb._mass;
	  _pos = rb._pos;
	  _vel = rb._vel;
	  _acc = rb._acc;
	  _rot = rb._rot;
    }
    return *this;
  }
  // ************************************************** RigidBody::destruction
  virtual ~RigidBody() {}
  // **************************************************** RigidBody::add_force
  void add_force( const TVec3& force )
  {
	_acc += force;
  }
  // ******************************************************* RigidBody::update
  void update( const TNumber& delta_time )
  {
	// divide _acc by mass
	_acc *= (delta_time / _mass);
	_vel += _acc;
	auto delta_pos = _vel;
	delta_pos *= delta_time;
	_pos += delta_pos;

	// Reset acceleration
	_acc = {0,0,0};
  }
  // ********************************************************** RigidBody::str
  std::string str_dump()
  {
	std::stringstream ss;
	ss << "m=" << _mass << std::endl;
    ss << "p=" << glm::to_string(_pos);
	ss << " v=" << glm::to_string(_vel);
	// ss << " a=" << glm::to_string(_acc);
	ss << std::endl;

	ss << "r=" << glm::to_string(_rot);
	// in euler angles
	auto angles = glm::degrees(glm::eulerAngles(_rot));
	ss << " [" << glm::to_string(angles) << "]";
	
	  

	return ss.str();
  }
  // ***************************************************** RigidBody::to_bases
  /** Express a global TVec3 to the local Base */
  TVec3 to_local( const TVec3& global_pos )
  {
	// Substract base position
	auto local_pos = global_pos - _pos;
	// unrotate
	local_pos = glm::rotate( glm::inverse(_rot), local_pos);
	return local_pos;
  }
  /** Express a local TVec3 to the global Base */
  TVec3 to_global( const TVec3& local_pos )
  {
	// Apply base rotation
	auto global_pos = glm::rotate( _rot, local_pos );
	// Add base position
	global_pos += _pos;

	return global_pos;
  }
public:
  // **************************************************** RigidBody::attributs
  TMass _mass;
  TVec3 _pos;
  TVec3 _vel, _acc;
  TRot  _rot;
};

}; // namespace physics

#endif // RIGID_BODY_HPP
