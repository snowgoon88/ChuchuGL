// -*- coding: utf-8 -*-

/**
   test-007-snd.cpp
 * Essai de jouer un fichier .ogg avec PortAudio et Vorbisfile.
 */
#include <chrono>                         // std::chrono
#include <thread>                         // std::thread

#include "snd_player.hpp"
#include "snd_piece.hpp"
#include <iostream>                       // std::cout

// ********************************************************************** main
int main( int argc, char *argv[] )
{
  SND::Player player;
  
  std::cout << "__CREATION SNDPlayer__" << std::endl;

  SND::Piece sound( "data/31304__erdie__imp04.ogg", true );
  SND::Piece music( "ressources/15-sonic-team-presents.ogg", true );

  //sound.inverse();
  //music.inverse();

  std::cout << "__PLAY__" << std::endl;
  player.play( music );
  player.play( sound );
  std::cout << "__END__" << std::endl;

  std::cout << "__OPEN STREAM__" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "__END STREAM__" << std::endl;
  return 0;
}

