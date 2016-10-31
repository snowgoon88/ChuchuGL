// -*- coding: utf-8 -*-

/**
 * test-014-snd.cpp
 * Play music using an external loop that feed the stream
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

  SND::Piece music( "ressources/02-multiplayer-theme.ogg", true );



  std::cout << "__OPEN STREAM__" << std::endl;
  SND::Player snd;
  snd.open_stream();
  std::cout << "pause 2s" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));

  std::cout << "add piece" << std::endl;
  snd.add_piece( music );

  std::cout << "enter loop" << std::endl;
  // loop of exactly 20ms
  unsigned int count = 0;
  while( true ) {
	auto start_proc = std::chrono::steady_clock::now();

	snd.feed();

	count += 1;
	if( count == 15 ) {
	  std::cout << "mute" << std::endl;
	  snd.mute( true );
	}
	if( count == 80 ) {
	  snd.mute( false );
	  std::cout << "unmute" << std::endl;
	}
	if( count == 200 ) {
	  snd.set_volume( 0.1f );
	  std::cout << "volume " << snd.get_volume() << std::endl;

	}
	if( count > 2000 ) {
	  break;
	}
	// wait
	auto end_proc = std::chrono::steady_clock::now();
	// wait period 
	std::chrono::duration<double> elapsed_seconds = end_proc - start_proc;
	std::this_thread::sleep_for(std::chrono::milliseconds(20)
								- elapsed_seconds );
  }
  std::cout << "__END STREAM__" << std::endl;
  snd.close_stream();
					  
  return 0;
}

