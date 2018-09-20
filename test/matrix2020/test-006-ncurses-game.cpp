/* -*- coding: utf-8 -*- */

/** 
 * test using ncurses for a simple game loop using only console
 */

//#include <stdio.h>
#include <iostream>
#include <ncurses.h>

// Model
#include <matrix2020/m_scene.hpp>
#include <matrix2020/fov_hamming.hpp>

using namespace matrix2020;

Environment* _env;
Scene*       _scene;
FOVHamming*  _fov;
Pos*         _hacker_pos;

void init()
{
  _env = new Environment();
  _env->load_from_txt( "data/matrix00.txt" );
  _scene = new Scene( *_env );
  _scene->init();
  _scene->_hacker->_pos = {5,5};
  _hacker_pos = new Pos( _scene->_hacker->pos() );
  _fov = new FOVHamming( *_env, *_hacker_pos, 4 ); 
}
void free_memory()
{
  delete _scene;
  delete _env;
}
/** display Scene using ncurses */
void nc_display()
{
  // TODO: BEURK
  _hacker_pos->x = _scene->_hacker->pos().x;
  _hacker_pos->y = _scene->_hacker->pos().y;
  _fov->refresh();
  
  clear();
  printw( _scene->str_view().c_str() );
  printw( "\n");
  printw( _fov->str_view().c_str() );
  printw( "\n");
  printw( "Choose Dir or <BACKSPACE> : " );


}

int main(int argc, char *argv[])
{
  // init Game
  init();
  
  // init NCURSES
  initscr();
  noecho();
  cbreak();               // line buffering disabled, no need for ENTER
  keypad(stdscr, TRUE);	  // We get F1, F2, ARROW, etc..

  try {
  bool end = false;
  while (not end) {
    nc_display();
    
    auto c = getch();

    switch (c) {
    case KEY_UP:
    case 'z':
      _scene->on_key_up();
      break;
    case KEY_DOWN:
    case 's':
      _scene->on_key_down();
      break;
    case KEY_LEFT:
    case 'q':
      _scene->on_key_left();
      break;
    case KEY_RIGHT:
    case 'd':
      _scene->on_key_right();
      break;
    case KEY_BACKSPACE:
      end = true;
      printw( "This is the end\n" );
      break;
    default:
      printw( "--%c--\n", c );
    }
    // NCURSES on real screen
    refresh();
  }
  }
  catch (...) {
     // end NCURSES mode
    refresh();
    clrtoeol();
    endwin();

    std::cout << "UNKNOWN Exception : exit "<< std::endl;
    exit(1);
  }
  
  // end NCURSES mode
  refresh();
  clrtoeol();
  endwin();
  free_memory();
  
  return 0;
}
