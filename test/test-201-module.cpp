/* -*- coding: utf-8 -*- */

/** 
 * Test embed lua to load a module and call a function
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
  luaL_dofile(L, "data/hello_module.lua");    // load file
  lua_setglobal( L, "hello_module" ); // Pop top of stack and set global var
  lua_settop(L, 0);               // clean stack

  // Run the sayhi() function.
  lua_getglobal(L, "hello_module"); // push to stack
  lua_getfield(L, -1, "sayhi");  // -1 means stack top
  lua_call(L, 0, 0);            // call function at top; 0, 0 = #args, #retvals
  
  return 0;
}
