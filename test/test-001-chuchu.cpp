/* -*- coding: utf-8 -*- */

/** 
 * Création d'un Chuchu et affichage.
 */

#include <iostream>                       // std::cout

#include <chuchu.hpp>

//******************************************************************************
int main( int argc, char *argv[] )
{
  Chuchu chu;
  std::cout << chu.str_dump() << std::endl;

  return 0;
}
