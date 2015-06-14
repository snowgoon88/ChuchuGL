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

    // fin
    ov_clear( &ogg_file );
  };
};

#endif // SND_PLAYER_HPP
