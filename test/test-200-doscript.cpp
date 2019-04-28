/* -*- coding: utf-8 -*- */

/** 
 * Test embed lua to run simple script
 * From : https://www.oreilly.com/library/view/creating-solid-apis/9781491986301/ch01.html
 * Compile : cc/g++ test-200-doscript.cpp -o test-200-doscript -llua -L../lua -I../lua -lm -ldl
 */
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"


// ********************************************************************** main
int main(int argc, char *argv[])
{
  lua_State *L = luaL_newstate(); // create lua state
  luaL_openlibs(L);               // open standart lua library
  luaL_dofile(L, "data/hello.lua");    // load file
  
  return 0;
}


