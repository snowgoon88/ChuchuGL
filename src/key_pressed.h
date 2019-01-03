/* -*- coding: utf-8 -*- */

#ifndef KEY_PRESSED_HPP
#define KEY_PRESSED_HPP

/** 
 * Implement a function to test if keyboard was hit.
 * Blocking wait :  wait_keypressed()
 */

#include <iostream>
//#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Non blocking test to see if keyboard was hit
 * by Vinay Sajip, comments by Lucas
 * https://stackoverflow.com/questions/1449324/how-to-simulate-press-any-key-to-continue/9419951#9419951
 *
 * There is also a nice C++ implementation at 
 * https://gist.github.com/johnsyweb/1910950
 *
 * Example : wait for a key
 * printf("Press any key:");
 * fflush(stdout);
 * while (!kbhit())
 *     ;
 * puts("Done");
 */
int kbhit(void)
{
  // the struct termios stores all kinds of flags which can
  // manipulate the I/O Interface
  // => an old one to save the old settings and a new 
  struct termios oldt, newt;
  int ch;
  int oldf;

  // tcgetattr gets the parameters of the current terminal
  // STDIN_FILENO will tell tcgetattr that it should write the settings
  // of stdin to oldt
  tcgetattr(STDIN_FILENO, &oldt);
  //now the settings will be copied
  newt = oldt;

  //two of the c_lflag will be turned off
  //ECHO which is responsible for displaying the input of the user in the terminal
  //ICANON is the essential one!
  //Normally this takes care that one line at a time will be processed
  //that means it will return if it sees a "\n" or an EOF or an EOL
  newt.c_lflag &= ~(ICANON | ECHO);

  //Those new settings will be set to STDIN
  //TCSANOW tells tcsetattr to change attributes immediately.
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  // The non-blocking part I guess
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  // test keyboard/console
  ch = getchar();

  //the old settings will be written back to STDIN
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

void wait_keypressed()
{
  int c;
  while((c = getchar()) != '\n' && c != EOF)
		/* discard */ ;
  std::cout << "Press any key to continue..." << std::endl;
  fflush(stdout);
  while (!kbhit()) {
    sleep(0);
  }
}

#endif // KEY_PRESSED_HPP
