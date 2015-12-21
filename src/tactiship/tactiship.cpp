/* -*- coding: utf-8 -*- */

/** 
 * Test le GL3DScreen avec une tête de flèche.
 */

#include <iostream>                       // std::cout

#include <gl_engine.hpp>
#include <gl_3dscreen.hpp>
#include <trajectory.hpp>

// Model
#define NB_POS 8
// *************************************************************** Traj cercle
Trajectory make_circle( const Position& center = {0,0,0}, float radius=1.f )
{
  Trajectory traj;

  for( unsigned int i = 0; i < NB_POS; ++i) {
    float angle = 2.f * M_PI * (float) i / float (NB_POS);
    Position pos = center + Position{radius*cosf( angle ), radius*sinf(angle), 0.f};
    // roll (around 0x), pitch (0y), yaw (0z)
    Rotation rot = glm::quat( glm::vec3{0,0,angle+M_PI/2.f});
    traj.push_back( Pose{pos,rot} );
  }

  return traj;
}

//******************************************************************************
int main( int argc, char *argv[] )
{
  std::cout << "__WINDOW" << std::endl;
  GLEngine app( "TactiShip", 640, 480);
  
  Trajectory traj = make_circle( {0,0,0}, 5.f );
  GL3DTrajectory viewer_traj(traj);

  std::cout << "__START_SCREEN" << std::endl;
  GL3DScreen trid_screen( app, viewer_traj);
  
  std::cout << "__GO" << std::endl;
  app.run<GL3DScreen,bool>( trid_screen );
  
  return 0;
}
