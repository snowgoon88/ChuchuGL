/* -*- coding: utf-8 -*- */

/** 
 * Un monde graphique
 */

#include <iostream>                       // std::cout

#include <gl_window.hpp>
#include <gl_controler.hpp>

// Parsing command line options
#include <boost/program_options.hpp>
namespace po = boost::program_options;

unsigned int _nb_player = 0;

// ******************************************************************* options
void setup_options(int argc, char **argv)
{
  po::options_description desc("Options");
  desc.add_options()
    ("help,h", "produce help message")
    ("nb_player,n", po::value<unsigned int>(), "nb de joueur par défaut")
    ;

  // Options en ligne de commande
  po::options_description cmdline_options;
  cmdline_options.add(desc);

  // Options qui sont 'après'
  po::positional_options_description pod;
  //pod.add( "data_file", 1);
  
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

  if (vm.count("nb_player")) {
    _nb_player = vm["nb_player"].as<unsigned int>();
  }
}

//******************************************************************************
int main( int argc, char *argv[] )
{
  setup_options( argc, argv );
  
  std::cout << "__WINDOW" << std::endl;
  GLWindow app;

  std::cout << "__CREATION" << std::endl;
  app.init();

  std::cout << "__SET_CONTROLER" << std::endl;
  app.set_controler( _nb_player );

  app.render();

  return 0;
}
