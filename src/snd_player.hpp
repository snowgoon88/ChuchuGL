/* -*- coding: utf-8 -*- */

#ifndef SND_PLAYER_HPP
#define SND_PLAYER_HPP

/** 
 * Utilise PortAudio et Vorbisfile pour jouer des fichiers .ogg.
 */

#include <portaudio.h>

#include <sstream>                        // std::stringstream
#include <iostream>                       // std::cout
// ***************************************************************************
// ***************************************************************** SNDPlayer
// ***************************************************************************
class SNDPlayer
{
public:
  // **************************************************************** creation
  SNDPlayer() 
  {
    // Initialise PortAudio
    PaError err = Pa_Initialize();
    if( err != paNoError ) {
      std::cerr << "SNDPlayer() pb avec initialisation de PortAudio." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( err );
    };
    // On cherche des infos sur le Device par défaut
    PaDeviceIndex id = Pa_GetDefaultOutputDevice();
    if( id == paNoDevice ) {
      std::cerr << "SNDPlayer() pas de device par défaut." << std::endl;
    }
    else {
      const PaDeviceInfo* dev_info = Pa_GetDeviceInfo( id );
      if( dev_info == NULL ) {
	std::cerr << "SNDPlayer() device "<<id<<" n'a pas d'info" << std::endl;
      }
      else {
	std::stringstream str;
	str << "__DEVICE INFO__" << std::endl;
	str << "  hostAPI = " << dev_info->hostApi << std::endl;
	str << "  maxInputChan = " << dev_info->maxInputChannels;
	str << " maxOutputChan = " << dev_info->maxOutputChannels << std::endl;
	str << "  InputLatency : " << dev_info->defaultLowInputLatency << " - " << dev_info->defaultHighInputLatency << std::endl;
	str << "  OutputLatency : " << dev_info->defaultLowOutputLatency << " - " << dev_info->defaultHighOutputLatency << std::endl;
	str << "  sampleRate = " << dev_info->defaultSampleRate << std::endl;
	std::cout << str.str() << std::endl;
      }
    }
  };
  // ************************************************************* destruction  
  virtual ~SNDPlayer()
  {
    PaError err = Pa_Terminate();
    if( err != paNoError ) {
      std::cerr << "SNDPlayer() pb avec terminaison de PortAudio." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( err );
    };
  };
};

#endif // SND_PLAYER_HPP
