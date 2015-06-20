/* -*- coding: utf-8 -*- */

/**
 * test-008-joystick.cpp
 *
 * Utilise GLFW pour lister les joysticks branché et leur état
 *
 * ACRUX HAMA X-Style Pad (si bouton allumé) !!
 * LR = axes[5] in -1; 1
 * UD = axes[6] in -1; 1
 * btn URDL = 3;1;0;2
 */

#include <GLFW/glfw3.h>
#include <iostream>                     // std::cout
#include <chrono>                      // std::chrono
#include <thread>                      // std::thread


void error_callback(int error, const char* description)
{
  std::cerr <<  description << std::endl;
}
//******************************************************************************
int main( int argc, char *argv[] )
{
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  int count; // nb_of buttons or axes
  const unsigned char* state;
  const float* axes;
  while( true ) {
    for( int joy = GLFW_JOYSTICK_1; joy <= GLFW_JOYSTICK_LAST; ++joy) {
      if( glfwJoystickPresent( joy )) {
	std::cout << "Joystick " << joy << " : ";
	std::cout << glfwGetJoystickName( joy ) << std::endl;
	state = glfwGetJoystickButtons( joy, &count);
	for( int i = 0; i < count; ++i) {
	  std::cout << "  btn["<<i<<"]="<<state[i] << std::endl;
	}
	axes = glfwGetJoystickAxes( joy, &count );
	for( int i = 0; i < count; ++i) {
	  std::cout << "  axe["<<i<<"]="<<axes[i] << std::endl;
	}
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
				
  return 0;
}

