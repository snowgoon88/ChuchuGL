/* -*- coding: utf-8 -*- */

/** 
 * Test le GLEngine par défaut.
 */

#include <iostream>                       // std::cout

#include <gl_engine.hpp>
#include <gl_screen.hpp>
#include <gl_controler.hpp>
#include <gl_game.hpp>

// Model
#include <world.hpp>
// Parsing command line options
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// ********************************************************************* Param
using Param = struct param {
  unsigned int nb_player;
  std::string data_file;
  // TODO : GLControler::Association
};
// ******************************************************************* Globals
Param _param = {0, "data/world_6x5.json"};
// ************************************************************* setup_options
void setup_options(int argc, char **argv)
{
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "produce help message")
    ("nb_player,n", po::value<unsigned int>(&(_param.nb_player)),
	 "nb de joueur par défaut")
	("data_file", po::value<std::string>(&(_param.data_file)),
	 "json file with world definition")
    ;

  // Options en ligne de commande
  po::options_description cmdline_options;
  cmdline_options.add(desc);

  // Options qui sont 'après'
  po::positional_options_description pod;
  pod.add( "data_file", -1);
  
  // Parse
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(desc).positional(pod).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
      std::cout << desc << std::endl;
    exit(1);
  }
}
//******************************************************************************
int main( int argc, char *argv[] )
{
  // Options
  setup_options( argc, argv );
  std::cout << "__PARAM" << std::endl;
  std::cout << "  nb_player=" << _param.nb_player << std::endl;
  std::cout << "  datafile =" << _param.data_file << std::endl;

  // Controllers must be configured
  if( _param.nb_player == 0 ) {
	std::cout << "__WINDOW" << std::endl;
	GLEngine app( "Chuchu Rocket", 1128, 752);
  
	std::cout << "__START_SCREEN" << std::endl;
	GLScreen start_screen( app );
  
	std::cout << "__GO" << std::endl;
	bool start = app.run<GLScreen,bool>( start_screen );
  
	if( start ) {
	  World world( "data/world_6x5.json" ); 
	  // World world( "data/world_pvp001.json" );
	  GLControler controler_screen( app, world );
	  controler_screen.init_default(1);
	  bool ready = app.run<GLControler,bool>( controler_screen );
    
	  if( ready ) {
		GLGame game_screen( app, world, controler_screen );
		bool finished = app.run<GLGame,bool>( game_screen );
	  }
	}
  }
  // Controllers set by options
  else {
	GLEngine app( "Chuchu Rocket Quick", 1128, 752 );
	World world( _param.data_file );
	GLControler controler( app, world );
	controler.init_default( _param.nb_player );
	GLGame game_screen( app, world, controler );
	bool finished = app.run<GLGame,bool>( game_screen );
  }
  
  return 0;
}
