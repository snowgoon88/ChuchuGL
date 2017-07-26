/* -*- coding: utf-8 -*- */

/** 
 * Test physics::rigid_body
 */
#include <iostream>                       // std::cout

#include <physics/rigid_body.hpp>
#include <physics/engine.hpp>
// ********************************************************************** main
int main(int argc, char *argv[])
{
  physics::RigidBody rb;
  std::cout << "__START gravity fall" << std::endl;
  std::cout << rb.str_dump() << std::endl;
  rb._vel = physics::TVec3( 1,0,0 );

  // apply gravity for 10*0.5 seconds
  auto grav = physics::TVec3(0,0,-1);
  grav *= 9.81;
  for( unsigned int i = 0; i < 10; ++i) {
	auto force = grav;
	grav *= rb._mass;
    rb.add_force( force );
  	rb.update( 0.5 );
  	std::cout << "__UPDATE " << i << std::endl;
  	std::cout << rb.str_dump() << std::endl;
  }

  std::cout << "__ ENGINE" << std::endl;
  physics::Engine eng( true );
  auto pt = physics::RigidBodyPtr( new physics::RigidBody() );
  pt->_vel = physics::TVec3( 1,0,0 );
  eng._bodies.push_back( pt );

  std::cout << "__START " << std::endl;
  std::cout << pt->str_dump() << std::endl;
  for( unsigned int i = 0; i < 10; ++i) {
    eng.update( 0.5 );
	std::cout << "__UPDATE " << i << std::endl;
  	std::cout << pt->str_dump() << std::endl;
  }

  
  return 0;
}
