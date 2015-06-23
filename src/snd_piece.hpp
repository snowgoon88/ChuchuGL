/* -*- coding: utf-8 -*- */

#ifndef SND_PIECE_HPP
#define SND_PIECE_HPP

/** 
 * Un SND::Piece est essentiellement un buffer qui contient des données
 * 'PCM' pouvant être jouée par le SND::Player.
 *
 * Initialié par un ficher .ogg, le format doit être:
 * - 1 ou 2 channels => transformé en 2 channel interleaved (stéréo)
 * - 44100 Hz
 * - des float entre -1 et 1.
 * 
 * TODO Chaque morceau peut-être joué avec un gain.
 */

#include <string>
#include <sstream>                        // std::stringstream
#include <iostream>                       // std::cout

#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

namespace SND 
{
  // TODO Exception !
// ******************************************************************** global
#include "snd_global.hpp"
// ***************************************************************************
// **************************************************************** SND::Piece
// ***************************************************************************
class Piece
{
public:
  // **************************************************************** creation
  Piece( std::string filename, bool verb=false ) :
    _ogg_buffer(nullptr), _ogg_pcm_length(0)
  {
    // structure avec les infos sur le Fichier Ogg
    OggVorbis_File ogg_file;
    // Ouvre le fichier Ogg
    int err = ov_fopen( filename.c_str(), &ogg_file );
    if( err < 0 ) { // fail
      Piece::error( "SND::Piece() ogg.open", err);
    }
    
    if( verb ) {
      // Give info for the current bitsrream (-1)
      vorbis_info *ogg_info = ov_info( &ogg_file, -1);
      std::stringstream str;
      
      str << "__OGG INFO__ : " << filename << std::endl;
      str << "  nb streams : " << ov_streams( &ogg_file ) << std::endl;
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
    }


    // Ne gère que les fichier avec un seul stream
    if( ov_streams( &ogg_file ) != 1 ) {
      std::cerr << "SND::Piece() ogg.nb_stream = " << ov_streams( &ogg_file ) << std::endl;
      Piece::error( "SND::Piece() wrong nb of stream", 0);
      exit( EXIT_FAILURE );
    }
    
    // info sur le fichier
    vorbis_info *ogg_info = ov_info( &ogg_file, -1);
    // Ne gère que bitrate de BIT_RATE
    if( ogg_info->rate != BIT_RATE ) {
      std::cerr << "SND::Piece() ogg.bitrate = " << ogg_info->rate << std::endl;
      Piece::error( "SND::Piece() wrong bitrate", 0);
      exit( EXIT_FAILURE );
    }
    // Ne gère que NB_CHANNEL channels au max
    int nb_channel = ogg_info->channels;
    if( nb_channel > NB_CHANNEL ) {
      std::cerr << "SND::Piece() ogg.nb_channel = " << nb_channel << std::endl;
      Piece::error( "SND::Piece() wrong nb of channel", 0);
      exit( EXIT_FAILURE );
    }

    // Decode le fichier
    // Buffer pour la sortie de la taille du truc
    // HACK : ajoute 1s=BIT_RATE data = à 0 à la fin du fichier !!!
    _ogg_pcm_length = ov_pcm_total( &ogg_file,-1) + BIT_RATE;
    // Prépare les buffers de donnée
    _ogg_buffer = new float[NB_CHANNEL * _ogg_pcm_length];
    
    float **pcm;
    int current_section;
    int ogg_idx = 0;
    bool ogg_end = false;
    while( not ogg_end ) {
      long ret = ov_read_float(&ogg_file, &pcm, 1024, &current_section);
      if( ret < 0 ) { //error
	Piece::error( "SND::Piece().decode", ret);
      }
      else if( ret == 0 ) {
	ogg_end = true;
      }
      else {
	// Prépare interleaved
	// Si un seul channel, alors on copie ce channel dans les
	// deux interleaved channel
	// => nb_channel-1 = 0, ce qui fait bien une copie du même
	//    pcml deux fois de suite
	for( unsigned int j = 0; j < (unsigned int) ret; ++j) {
	  _ogg_buffer[ogg_idx++] = pcm[0][j];
	  _ogg_buffer[ogg_idx++] = pcm[nb_channel-1][j];
	}
      }
    }	  
    // Ajoute à la fin
    for( unsigned int j = 0; j < BIT_RATE; ++j) {
      _ogg_buffer[ogg_idx++]= 0.f;
      _ogg_buffer[ogg_idx++]= 0.f;
    }
  
    // fin
    ov_clear( &ogg_file );
  }
  // ************************************************************* destruction
  virtual ~Piece()
  {
    if( _ogg_buffer ) delete _ogg_buffer;
  };
  // *************************************************************** operation
  /** Sera joué en sens inverse */
  void inverse()
  {
    float swap;
    for( unsigned int i = 0; i < size()/2; ++i) {
      swap = _ogg_buffer[i];
      _ogg_buffer[i] = _ogg_buffer[size()-1-i];
      _ogg_buffer[size()-1-i] = swap;
    }
  };
  /** Valeur opposée */
  void opposed()
  {
    for( unsigned int i = 0; i < size(); ++i) {
      _ogg_buffer[i] = - _ogg_buffer[i];
    }
  }
  /** Complémentaire en en valeur absolue */
  void comp()
  {
    for( unsigned int i = 0; i < size(); ++i) {
      if( _ogg_buffer[i] > 0)
	_ogg_buffer[i] = 1.f - _ogg_buffer[i];
      else
	_ogg_buffer[i] = -1.f - _ogg_buffer[i];
    }
  }
  // *************************************************************** attributs
  const unsigned int size() const { return _ogg_pcm_length;};
  const float* data() const { return _ogg_buffer; };
private:
  /** Buffer pour ogg décodé */
  float* _ogg_buffer;
  /** Taille du morceau */
  unsigned int _ogg_pcm_length;
  // ******************************************************************* error
  /** head_msg est au début du message d'erreur */
  static void error( const std::string& head_msg,  int err )
  {
    std::cerr << head_msg;
    switch( err ) {
    case OV_EREAD:
      std::cerr << " Read from medira returned error";
      std::cerr << std::endl;
      break;
    case OV_ENOTVORBIS:
      std::cerr << " Bitstream does not contain any Vorbis data";
      std::cerr << std::endl;
      break;
    case OV_EVERSION:
      std::cerr << " Vorbis version mismatch";
      std::cerr << std::endl;
      break;
    case OV_EBADHEADER:
      std::cerr << " Invalid Vorbis bitstream header.";
      std::cerr << std::endl;
      break;
    case OV_EFAULT:
      std::cerr << " Internal logic fault; indicates a bug or heap/stack corruption";
      std::cerr << std::endl;
      break;
    case OV_HOLE:
      std::cerr << " Interruption in data";
      std::cerr << std::endl;
      break;
    case OV_EBADLINK:
      std::cerr << " Invalid stream section !";
      std::cerr << std::endl;
      break;
    case OV_EINVAL:
      std::cerr << " Initial file headers couldn't be read or are corrupt, or that the initial open call for vf failed.";
      std::cerr << std::endl;
      break;
    case 0:
      // error in msg
      std::cerr << std::endl;
    default:
      std::cerr << " Unknown error";
      std::cerr << std::endl;
    }
  };
};

}; // namespace SND

#endif // SND_PIECE_HPP
