/* -*- coding: utf-8 -*- */

#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

/** 
 * A physics engine that update all rigidbody.
 * - a collection of RigidBodyPtr (list)
 * - gravity as global
 */
#include <list>
#include <physics/rigid_body.hpp>

// ***************************************************************************
// ******************************************************************** Engine
// ***************************************************************************
namespace physics {
class Engine
{
public:
  // ******************************************************** Engine::creation
  Engine( bool use_gravity = false ) :
	_use_gravity(use_gravity), _gravity( TVec3(0,0,-9.81))
  {
  }
  // ********************************************************** Engine::update
  void update( const TNumber& delta_time )
  {
	for (auto it = _bodies.begin(); it != _bodies.end(); ++it) {
	  auto bd = *it;
	  if( _use_gravity ) {
		// compute gravity force
		auto weight = _gravity;
		_gravity *= bd->_mass,
		  bd->add_force( weight );
	  }
	  bd->update( delta_time );
	}
  }
  // ******************************************************* Engine::attributs
  std::list<RigidBodyPtr> _bodies;
  bool _use_gravity;
  TVec3 _gravity;
}; // class Engine
}; // namespace physics

#endif // PHYSICS_ENGINE_HPP
