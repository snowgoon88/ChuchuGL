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
// ***************************************************************************
// ***************************************************************** SNDPlayer
// ***************************************************************************
class SNDPlayer
{
public:
  // **************************************************************** creation
  SNDPlayer() :
    _ogg_buffer(nullptr)
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
    if( _ogg_buffer ) delete _ogg_buffer;
  };
  // *************************************************************** play_file
  void play()
  {
    // Frequence de 44100 Hz
    // On essaie de passer SAMPLE_PER_FRAME=4410 samples toutes les 100ms
    const double SAMPLE_RATE = 44100;
    unsigned long SAMPLE_PER_FRAME = 4410;

    PaStream *pa_stream;
    PaError pa_err;
    /* Open an audio I/O stream. */
    pa_err = Pa_OpenDefaultStream( &pa_stream,
				   0, /* nb channel input */
				   1, /* nb channel output */
				   paInt16, /* 16 bit integer format */
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
      std::cerr << "SNDPlayer.play() ouverture de stream." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };

    // Start Stream
    pa_err = Pa_StartStream( pa_stream );
    if( pa_err != paNoError ) {
      std::cerr << "SNDPlayer.play() start stream." << std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };

    // Play
    bool play_end = false;
    unsigned long ogg_idx = 0;
    unsigned long frame_size = SAMPLE_PER_FRAME;
    std::cout << "__START PLAY of " << _ogg_pcm_length << " PCM sample" << std::endl;
    while( not play_end ) {
      // clock
      auto start_proc = std::chrono::steady_clock::now();
      // Combien de place il reste
      unsigned long pa_available = Pa_GetStreamWriteAvailable( pa_stream);
      
      // nourrit le pa_stream
      frame_size = SAMPLE_PER_FRAME;
      if( frame_size > pa_available ) {
	frame_size = pa_available;
      }
      if( ogg_idx + 2*frame_size >= _ogg_pcm_length ) {
	frame_size = (_ogg_pcm_length - ogg_idx)/2;
	play_end = true;
      }
      std::cout << "* avail=" << pa_available << " frame_size="<< frame_size << " idx=" << ogg_idx << std::endl;
      pa_err = Pa_WriteStream( pa_stream, 
			       &(_ogg_buffer[ogg_idx]), frame_size);
      if( pa_err != paNoError ) {
	std::cerr << "SNDPlayer.play() write stream idx="<< ogg_idx << std::endl;
	std::cerr << "            " << Pa_GetErrorText( pa_err );
      };
      ogg_idx += 2 * frame_size;
      
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
      std::cerr << "SNDPlayer.play() stop stream" << std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    /* -- don't forget to cleanup! -- */
    std::cout << "__CLOSE__" << std::endl;
    pa_err = Pa_CloseStream( pa_stream );
    if( pa_err != paNoError ) {
      std::cerr << "SNDPlayer.play() close stream "<< std::endl;
      std::cerr << "            " << Pa_GetErrorText( pa_err );
    };
  };
  // ************************************************************** read_files
  void read_ogg( std::string filename )
  {
    // structure avec les infos sur le Fichier Ogg
    OggVorbis_File ogg_file;
    // Ouvre le fichier Ogg
    int err = ov_fopen( filename.c_str(), &ogg_file );
    if( err < 0 ) { // fail
      switch( err ) {
      case OV_EREAD:
	std::cerr << "SNDPlayer.readogg() Read from medira returned error";
	std::cerr << std::endl;
	break;
      case OV_ENOTVORBIS:
	std::cerr << "SNDPlayer.readogg() Bitstream does not contain any Vorbis data";
	std::cerr << std::endl;
	break;
      case OV_EVERSION:
	std::cerr << "SNDPlayer.readogg() Vorbis version mismatch";
	std::cerr << std::endl;
	break;
      case OV_EBADHEADER:
	std::cerr << "SNDPlayer.readogg() Invalid Vorbis bitstream header.";
	std::cerr << std::endl;
	break;
      case OV_EFAULT:
	std::cerr << "SNDPlayer.readogg() Internal logic fault; indicates a bug or heap/stack corruptio";
	std::cerr << std::endl;
	break;
      default:
      	std::cerr << "SNDPlayer.readogg() Unknown error";
	std::cerr << std::endl;
      }
    }
    // Give info for the current bitsrream (-1)
    vorbis_info *ogg_info = ov_info( &ogg_file, -1);
    std::stringstream str;
    
    str << "__OGG INFO__ : nb streams : " << ov_streams( &ogg_file ) << std::endl;
    str << "  version " << ogg_info->version << std::endl;
    str << "  " << ogg_info->channels << " channel(s) avec bitrate = " << ogg_info->rate << std::endl;;
    str << "  birates (l/n/u) " << ogg_info->bitrate_lower << "/" << ogg_info->bitrate_nominal << "/" << ogg_info->bitrate_upper << std::endl;
    str << "  taille " << ov_pcm_total( &ogg_file,-1) << " samples" << std::endl;
    str << "  compressed " << ov_raw_total( &ogg_file, -1) << std::endl;
    str << "  time " << ov_time_total( &ogg_file, -1) << std::endl;
    // Les commentaires
    char **ogg_cmt = ov_comment( &ogg_file, -1)->user_comments;
    while( *ogg_cmt ) {
      str << "  * " << *ogg_cmt << std::endl;
      ++ogg_cmt;
    }
    str << "  Vendor : " << ov_comment( &ogg_file, -1)->vendor << std::endl;
    
    std::cout << str.str() << std::endl;

    // Decode le fichier
    // Buffer pour la sortie de la taille du truc
    // HACK : ajoute 1s de 0 à la fin du fichier !!!
    _ogg_pcm_length = ov_pcm_total( &ogg_file,-1) + 44100;
    _ogg_buffer = new char[2*_ogg_pcm_length];
    for( unsigned int i = 0; i < 2*_ogg_pcm_length; ++i) {
      _ogg_buffer[i] = 0;
    }

    char* ogg_buffer_ptr = _ogg_buffer;
    bool ogg_end = false;
    int current_section;
    long size_total = 0;
    while( not ogg_end ) {
      long ret = ov_read( &ogg_file, ogg_buffer_ptr, 4096,
			  0 /*little_endian*/, 2 /* 16bit samples*/,
			  1 /*signed*/,
			  &current_section );
      size_total += ret;
      //std::cout << "ov_read => " << ret << " in section=" << current_section << " size= " << size_total << std::endl;
      if( ret < 0 ) { //error
	switch( ret) {
	case OV_HOLE:
	  std::cerr << "SNDPlayer.readogg() Interruption in data";
	  std::cerr << std::endl;
	break;
	case OV_EBADLINK:
	  std::cerr << "SNDPlayer.readogg() Invalid stream section !";
	  std::cerr << std::endl;
	break;
	default:
	  std::cerr << "SNDPlayer.readogg() Unknown error";
	  std::cerr << std::endl;
	}
      }
      else if( ret == 0 ) {
	ogg_end = true;
      }
      else {
	ogg_buffer_ptr += ret;
      }
    }

    // fin
    ov_clear( &ogg_file );
  };
  // *************************************************************** attributs
private:
  /** Buffer pour ogg décodé */
  char* _ogg_buffer;
  ogg_int64_t _ogg_pcm_length;
};

#endif // SND_PLAYER_HPP
