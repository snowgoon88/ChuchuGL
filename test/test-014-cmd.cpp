/* -*- coding: utf-8 -*- */

/** 
 * test MCommandLine.
 */

#include "m_commandline.hpp"
#include <iostream>

void test_creation()
{
  MCommandLine cmd;
  std::wcout << "__CREATION" << std::endl;
  std::wcout << cmd.wstr_dump() << std::endl;

  std::wcout << "__ADD" << std::endl;
  cmd.add( u'A' );
  std::wcout << cmd.wstr_dump() << std::endl;

  std::wcout << "__ADD" << std::endl;
  cmd.add( u'É' );
  std::wcout << cmd.wstr_dump() << std::endl;

  std::wcout << "__ADD" << std::endl;
  cmd.add( u'b', {1,0,0} );
  std::wcout << cmd.wstr_dump() << std::endl;

  cmd._cursor -= 4;
  std::wcout << "__ADD -4 should NOT be allowed !! => private" << std::endl;
  cmd.add( u'ç' );
  std::wcout << cmd.wstr_dump() << std::endl;
}

int main(int argc, char *argv[])
{
  // config wcout using locale
  constexpr char locale_name[] = "";
  std::locale::global(std::locale(locale_name));
  std::wcout.imbue(std::locale());
    
  test_creation();
  return 0;
}
