// Can display utf8 but internal are chars and not useable with Glyph...

#include <string>
#include <iostream>

#include <locale>
#include <codecvt>

int main(int argc, char *argv[])
{
  std::string utf8 = std::string( "ABÉgiéç" );
  std::cout << utf8 << std::endl;

  for( auto itt = utf8.begin(); itt != utf8.end(); ++itt)  {
    char16_t c = static_cast<char16_t>( *itt );
    std::cout << *itt << " = " << c << std::endl;
  }

  // // conversion to utf16
  // std::wstring_convert<std::codecvt_utf8_utf16<char16_t>> converter;
  // std::wstring wstr = converter.from_bytes(utf8);

  // std::wcout << wstr << std::endl;
  
  return 0;
}
