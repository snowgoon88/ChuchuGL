/* -*- coding: utf-8 -*- */

/** 
 * Test le GL3DScreen avec une tête de flèche.
 */

#include <iostream>                       // std::cout

#include <gl_3dengine.hpp>
#include <gl_3dsimu.hpp>
//#include <trajectory.hpp>

// // Model
// #define NB_POS 64
// // *************************************************************** Traj cercle
// Trajectory make_circle( const Position& center = {0,0,0}, float radius=1.f )
// {
//   Trajectory traj;

//   for( unsigned int i = 0; i < NB_POS; ++i) {
//     float angle = 2.f * M_PI * (float) i / float (NB_POS);
//     Position pos = center + Position{radius*cosf( angle ), radius*sinf(angle), angle};
//     // roll (around 0x), pitch (0y), yaw (0z)
//     Rotation rot = glm::quat( glm::vec3{-2*angle,0,angle+M_PI/2.f});
//     if( i % 8 == 0) {
//       traj.push_back( Pose{pos,rot,true} );
//     }
//     else {
//       traj.push_back( Pose{pos,rot,false} );
//     }
//   }

//   return traj;
// }

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GL3DEnginePtr app = GL3DEnginePtr(new GL3DEngine("Simulator", 640, 480));
  
  std::cout << "__START_SCREEN" << std::endl;
  GL3DSimu trid_screen( app );//, viewer_traj);
  
  std::cout << "__GO" << std::endl;
  app->run<GL3DSimu,bool>( trid_screen );
  
  return 0;
}
