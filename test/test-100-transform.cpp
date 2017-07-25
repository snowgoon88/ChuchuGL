/* -*- coding: utf-8 -*- */

/** 
 * Test physics::rigid_body base transformation from local to global and
 * vice versa.
 */
#include <iostream>                       // std::cout

#include <physics/rigid_body.hpp>

// ********************************************************************** main
int main(int argc, char *argv[])
{
  physics::RigidBody rb;
  rb._pos = physics::TVec3( 1, 1, 0);
  // roll (around 0x), pitch (0y), yaw (0z)
  rb._rot = physics::TRot( glm::vec3(M_PI/4.0,0,M_PI/4.0));
  auto ux = physics::TVec3(1, 0, 0);
  auto uy = physics::TVec3(0, 1, 0);
  auto uz = physics::TVec3(0, 0, 1);
  std::cout << "__START" << std::endl;
  std::cout << "  BASE at ";
  std::cout << rb.str_dump() << std::endl;

  std::cout << "__TRANSFORM" << std::endl;
  std::cout << "ux=" << glm::to_string(ux) << "->" << glm::to_string(rb.to_global(ux)) << std::endl;
  std::cout << "uy=" << glm::to_string(uy) << "->" << glm::to_string(rb.to_global(uy)) << std::endl;
  std::cout << "uz=" << glm::to_string(uz) << "->" << glm::to_string(rb.to_global(uz)) << std::endl;
  std::cout << "ux=" << glm::to_string(ux) << "<=" << glm::to_string(rb.to_local(ux)) << std::endl;
  std::cout << "uy=" << glm::to_string(uy) << "<=" << glm::to_string(rb.to_local(uy)) << std::endl;
  std::cout << "uz=" << glm::to_string(uz) << "<=" << glm::to_string(rb.to_local(uz)) << std::endl;
  
  return 0;
}
