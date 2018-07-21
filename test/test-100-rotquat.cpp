/* -*- coding: utf-8 -*- */

/** 
 * Test GLM quaternion and rotations.
 */
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>     // definition
#include <glm/gtx/quaternion.hpp>     // rotation of vec3

// ************************************************* quaternion from 2 vectors
// *********************************************** quaternion from 2 vectors
/* Build a unit quaternion representing the rotation
 * from u to v. The input vectors need not be normalised. */
glm::quat from_two_vectors(glm::vec3 u, glm::vec3 v)
{
  float norm_u_norm_v = glm::sqrt(glm::dot(u, u) * glm::dot(v, v));
  float real_part = norm_u_norm_v + glm::dot(u, v);
  glm::vec3 w;

  if (real_part < 1.e-6f * norm_u_norm_v) {
    /* If u and v are exactly opposite, rotate 180 degrees
     * around an arbitrary orthogonal axis. Axis normalisation
     * can happen later, when we normalise the quaternion. */
    real_part = 0.0f;
    w = abs(u.x) > abs(u.z) ? glm::vec3(-u.y, u.x, 0.f)
      : glm::vec3(0.f, -u.z, u.y);
  }
  else {
    /* Otherwise, build quaternion the standard way. */
    w = glm::cross(u, v);
  }

  return glm::normalize(glm::quat(real_part, w.x, w.y, w.z ));
}

// ********************************************************************** main
int main(int argc, char *argv[])
{
  std::cout << "__ default creation" << std::endl;
  glm::quat q0;

  std::cout << "  q0 = " << glm::to_string(q0) << std::endl;
  // in euler angles
  auto angles = glm::degrees(glm::eulerAngles( q0 ));
  std::cout << "    : "  << glm::to_string(angles) << std::endl;

  glm::quat q1(glm::vec3(0,0,0));
  std::cout << "  q1 = " << glm::to_string(q1) << std::endl;
  // in euler angles
  angles = glm::degrees(glm::eulerAngles( q1 ));
  std::cout << "    : "  << glm::to_string(angles) << std::endl;

  glm::quat q2 = glm::angleAxis((float) M_PI/2.0f, glm::vec3{0.f, 0.f, 1.f} );
  std::cout << "  q2 = " << glm::to_string(q2) << std::endl;
  // in euler angles
  angles = glm::degrees(glm::eulerAngles( q2 ));
  std::cout << "    : "  << glm::to_string(angles) << std::endl;

  glm::vec3 Ox{1.f, 0.f, 0.f};
  glm::vec3 Oy{0.f, 1.f, 0.f};
  glm::quat q3 = from_two_vectors( Ox, Oy );
  std::cout << "  q3 = " << glm::to_string(q3) << std::endl;
  // in euler angles
  angles = glm::degrees(glm::eulerAngles( q3 ));
  std::cout << "    : "  << glm::to_string(angles) << std::endl;
  
  return 0;
}


