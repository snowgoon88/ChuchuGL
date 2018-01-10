/* -*- coding: utf-8 -*- */

#ifndef BOID_BODY_HPP
#define BOID_BODY_HPP

/** 
 * A special kind of RigidBody. Added
 * - max_force : used to truncate force applied
 * - max_vel : used to truncate velocity
 */

#include <physics/rigid_body.hpp>
#include <memory>                     // std::*_ptr

// *********************************************** quaternion from 2 vectors
/* Build a unit quaternion representing the rotation
 * from u to v. The input vectors need not be normalised. */
physics::TRot quad_from_two_vectors(physics::TVec3 u, physics::TVec3 v)
{
  float norm_u_norm_v = glm::sqrt(glm::dot(u, u) * glm::dot(v, v));
  float real_part = norm_u_norm_v + glm::dot(u, v);
  physics::TVec3 w;

  if (real_part < 1.e-6f * norm_u_norm_v) {
    /* If u and v are exactly opposite, rotate 180 degrees
     * around an arbitrary orthogonal axis. Axis normalisation
     * can happen later, when we normalise the quaternion. */
    real_part = 0.0f;
    w = abs(u.x) > abs(u.z) ? physics::TVec3(-u.y, u.x, 0.f)
      : physics::TVec3(0.f, -u.z, u.y);
  }
  else {
    /* Otherwise, build quaternion the standard way. */
    w = glm::cross(u, v);
  }

  return glm::normalize(physics::TRot(real_part, w.x, w.y, w.z ));
}
// ***************************************************************************

// ***************************************************************************
// ****************************************************************** BoidBody
// ***************************************************************************
class BoidBody;
using BoidBodyPtr = std::shared_ptr<BoidBody>;
class BoidBody : public physics::RigidBody
{
public:
  // ****************************************************** BoidBody::creation
  BoidBody( physics::TNumber max_force = 10, physics::TNumber max_vel = 10,
            physics::TMass mass = 1 ) :
    physics::RigidBody(mass), _max_force(max_force), _max_vel(max_vel)
  {}
  /** copy creation */
  BoidBody( const BoidBody& bb ) :
    physics::RigidBody( bb ),
    _max_force( bb._max_force), _max_vel( bb._max_vel)
  {}
  /** assignment creation */
  BoidBody& operator=( const BoidBody& bb )
  {
    if(this != &bb ) {
      physics::RigidBody::operator=(bb);
      _max_force = bb._max_force;
      _max_vel = bb._max_vel;
    }
    return *this;
  }
  // *************************************************** BoidBody::destruction
  virtual ~BoidBody() {}
  // *********************************************************BoidBody::update
  virtual void update( const physics::TNumber& delta_time )
  {
    // truncate force
    auto ratio_force = glm::max( 1.0, glm::length( _acc ) / _max_force);
    _acc /= ratio_force;
    // truncate vel
    auto ratio_vel = glm::max( 1.0, glm::length( _vel ) / _max_vel);
    _vel /= ratio_vel;
    
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
    // align to speed vector
        physics::TVec3 ox{1.f, 0.f, 0.f};
    _rot = quad_from_two_vectors( ox, _vel );
  }

public:
  // ******************************************************BoidBody::attributs
  physics::TNumber _max_force;
  physics::TNumber _max_vel;
};

#endif // BOID_BODY_HPP
