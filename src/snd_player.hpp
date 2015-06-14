/* -*- coding: utf-8 -*- */

#ifndef SND_PLAYER_HPP
#define SND_PLAYER_HPP

/** 
 * Utilise PortAudio et Vorbisfile pour jouer des fichiers .ogg.
 */

#include <portaudio.h>
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

#include <sstream>                        // std::stringstream
#include <iostream>                       // std::cout
#include <chrono>                         // std::chrono
#include <thread>                         // std::thread

#include "snd_piece.hpp"

namespace SND
{
// ******************************************************************** global
#include "snd_global.hpp"
// ***************************************************************************
// *************************************************************** SND::Player
// ***************************************************************************
class Player
{
public:
  // **************************************************************** creation
  Player()
  {
    // Initialise PortAudio
    PaError err = Pa_Initialize();
    if( err != paNoError ) {
      std::cerr << "SND::Player() pb avec initialisation de PortAudio." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( err );
    };
    // On cherche des infos sur le Device par défaut
    PaDeviceIndex id = Pa_GetDefaultOutputDevice();
    if( id == paNoDevice ) {
      std::cerr << "SND::Player() pas de device par défaut." << std::endl;
    }
    else {
      const PaDeviceInfo* dev_info = Pa_GetDeviceInfo( id );
      if( dev_info == NULL ) {
	std::cerr << "SND::Player() device "<<id<<" n'a pas d'info" << std::endl;
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
  virtual ~Player()
  {
    PaError err = Pa_Terminate();
    if( err != paNoError ) {
      std::cerr << "SND::Player() pb avec terminaison de PortAudio." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( err );
    };
  };
  // *************************************************************** play_file
  void play( Piece& piece )
  {
    // Frequence de 44100 Hz
    // On essaie de passer SAMPLE_PER_FRAME=4096 samples toutes les 100ms
    const double SAMPLE_RATE = 44100;
    unsigned long SAMPLE_PER_FRAME = 4096;

    PaStream *pa_stream;
    PaError pa_err;
    /* Open an audio I/O stream. */
    pa_err = Pa_OpenDefaultStream( &pa_stream,
				   0, /* nb channel input */
				   2, /* nb channel output */
				   paFloat32, /* format, Interleaved */
				   SAMPLE_RATE,
				   SAMPLE_PER_FRAME,
				   /* frames per buffer, i.e. the number
				      of sample frames that PortAudio will
				      request from the callback. Many apps
				      may want to use
				      paFramesPerBufferUnspecified, which
				      tells PortAudio to pick the best,
				      possibly changing, buffer size.*/
				   NULL, /* blocking mode */
				   NULL ); /*This is a pointer that will be passed to
					     your callback*/
    if( pa_err != paNoError ) {
      std::cerr << "SND::Player.play() ouverture de stream." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };

    // Start Stream
    pa_err = Pa_StartStream( pa_stream );
    if( pa_err != paNoError ) {
      std::cerr << "SND::Player.play() start stream." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };

    // Play
    bool play_end = false;
    unsigned long ogg_idx = 0;
    unsigned long frame_size = SAMPLE_PER_FRAME;
    const float* buffer = piece.data();
    const unsigned int length = piece.size();
    std::cout << "__START PLAY of " << length << " PCM sample" << std::endl;
    while( not play_end ) {
      // clock
      auto start_proc = std::chrono::steady_clock::now();
      // Combien de place il reste
      unsigned long pa_available = Pa_GetStreamWriteAvailable( pa_stream );
      
      // nourrit le pa_stream
      frame_size = SAMPLE_PER_FRAME;
      if( frame_size > pa_available ) {
	frame_size = pa_available;
      }
      if( ogg_idx + NB_CHANNEL*frame_size >= length ) {
	frame_size = (length - ogg_idx)/NB_CHANNEL;
	play_end = true;
      }
      std::cout << "* avail=" << pa_available << " frame_size="<< frame_size << " idx=" << ogg_idx << std::endl;
      pa_err = Pa_WriteStream( pa_stream, 
			       &(buffer[ogg_idx]), frame_size);
      if( pa_err != paNoError ) {
	std::cerr << "SND::Player.play() write stream idx="<< ogg_idx << std::endl;
	std::cerr << "            " << Pa_GetErrorText( pa_err );
      };
      ogg_idx += NB_CHANNEL * frame_size;
      
      // clock
      auto end_proc = std::chrono::steady_clock::now();
      // wait period 
      std::chrono::duration<double> elapsed_seconds = end_proc - start_proc;
      std::cout << "  * idx=" << ogg_idx << " with " << frame_size << " samples  in " <<  elapsed_seconds.count() <<" s" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(70)
				  - elapsed_seconds );
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    /* -- Now we stop the stream -- */
    std::cout << "__STOP__" << std::endl;
    pa_err = Pa_StopStream( pa_stream );
    if( pa_err != paNoError ) {
      std::cerr << "SND::Player.play() stop stream" << std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    /* -- don't forget to cleanup! -- */
    std::cout << "__CLOSE__" << std::endl;
    pa_err = Pa_CloseStream( pa_stream );
    if( pa_err != paNoError ) {
      std::cerr << "SND::Player.play() close stream "<< std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };
  };
  // *************************************************************** attributs
private:
};

}; // namespace SND

#endif // SND_PLAYER_HPP
