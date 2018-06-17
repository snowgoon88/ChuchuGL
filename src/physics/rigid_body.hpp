/* -*- coding: utf-8 -*- */

#ifndef RIGID_BODY_HPP
#define RIGID_BODY_HPP

/** 
 * Implementation of a 3D rigid body.
 * - mass
 * - pos : position [global] of center of mass, vector
 * - vel :velocity [global] vector
 * - acc :acceleration [global] vector
 *
 * - rot : rotation [global] as quaternion
 * - rot_spd : rotation_speed [global] as quaternion
 *
 * At every update, a Force can be applied to update its pos,vel,acc
 * - force : applied force for the given update interval
 * - drag : drag force proportionnal to speed^2
 *
 * As an "Interactive Object", some special force (thrust) can be applied
 * only once during a step of the physique engine.
 * - apply_thrust() + _thrust_applied
 */
#include <string>                     // std::string
#include <sstream>                    // std::stringdtream
#include <memory>                     // std::*_ptr

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>     // definition
#include <glm/gtx/quaternion.hpp>     // rotation of vec3

#define THRUST_VALUE 1.0

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
	_rot( glm::quat(glm::vec3(0,0,0)) ),
    _rot_spd( glm::quat(glm::vec3(0,0,0)) )
  {
    clear_applied();
  }
  /** copy creation */
  RigidBody( const RigidBody& rb ) :
	_mass( rb._mass), _pos( rb._pos),
	_vel( rb._vel), _acc( rb._acc ),
	_rot( rb._rot ),
    _thrust_applied( rb._thrust_applied )
  {
  }
  /** assignment creation */
  RigidBody& operator=( const RigidBody& rb )
  {
    if (this != &rb) { // protect against invalid self-assignment
	  _mass = rb._mass;
	  _pos = rb._pos;
	  _vel = rb._vel;
	  _acc = rb._acc;
	  _rot = rb._rot;

      _thrust_applied = rb._thrust_applied;
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
  void drag_force( const TNumber coef = 1 )
  {
    auto drag = glm::normalize( _vel );
    drag *= -coef;
    drag *= glm::length2( _vel );
    add_force( drag );
  }
  // ******************************************************* RigidBody::update
  virtual void update( const TNumber& delta_time )
  {
    // take drag into account
    drag_force( 0.1 );

    // update
	auto delta_pos = _vel;
	delta_pos *= delta_time;
	_pos += delta_pos;
    // divide _acc by mass
	_acc *= (delta_time / _mass);
	_vel += _acc;

	// Reset acceleration
	_acc = {0,0,0};

    // Clear Thrust
    clear_applied();

    // Rotation
    // target quaternion after 1s
    auto rot_target = _rot_spd * _rot;
    // interpolate using slerp
    _rot = glm::slerp( _rot, rot_target, (float)delta_time );
  }
  // ********************************************************** RigidBody::str
  std::string str_dump() const
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
  std::string str_display() const
  {
    std::stringstream ss;
    auto speed = glm::length( _vel );
    ss << "p=" << glm::to_string(_pos) << "v=" << glm::to_string(_vel );
    ss << " (" << speed << " m/s)";
    return ss.str();
  }
  // ******************************************************* RigidBody::thrust
  void apply_thrust( const TNumber value )
  {
    if( not _thrust_applied) {
      // vector aligned on local 0x
      TVec3 v_thrust { value, 0.0, 0.0};
      auto global_thrust = glm::rotate( _rot, v_thrust );
      add_force( global_thrust );
      _thrust_applied = true;
    }
  }
  void clear_applied()
  {
    _thrust_applied = false;
  }
  // ***************************************************** RigidBody::to_bases
  /** Express a global TVec3 to the local Base */
  TVec3 to_local( const TVec3& global_pos ) const
  {
	// Substract base position
	auto local_pos = global_pos - _pos;
	// unrotate
	local_pos = glm::rotate( glm::inverse(_rot), local_pos);
	return local_pos;
  }
  /** Express a local TVec3 to the global Base */
  TVec3 to_global( const TVec3& local_pos ) const
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
  TRot  _rot, _rot_spd;
  /* Specific for control */
  bool _thrust_applied;
};

}; // namespace physics

#endif // RIGID_BODY_HPP
