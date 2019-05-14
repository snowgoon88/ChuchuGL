// Can display utf8 but internal are chars and not useable with Glyph...
// but conversion is not so hard...

#include <string>
#include <iostream>

#include <locale>
#include <codecvt>

std::string str_from_code( int code )
{
  std::string res;
  
  if (code < 0x7F) {
    char c = (code & 0x7F);
    res.push_back( c );
  }
  else if (code < 0x7FF) {
    char c2 = (0x80 + (code & 0x3F));
    //std::cout << "c2 = " << std::hex << static_cast<unsigned int>(c2) << std::dec << std::endl;
    code = (code >> 6);
    char c1 = (0xC0 + (code & 0x1F));
    //std::cout << "c1 = " << std::hex << static_cast<unsigned int>(c1) << std::dec << std::endl;
    res.push_back( c1 );
    res.push_back( c2 );
  }
  else {
    std::cerr << "NOT IMPLEMENTER UTF with more than 2 bytes" << std::endl;
  }
  //res.push_back( 0 );
  return res;
}

void print_as_char( std::string::iterator itstart, std::string::iterator itend )
{
  std::string::iterator it = itstart;
  int code = 0;
  std::string utf8;
  char c;

  
  while (it != itend) {
    c = *it;
    // unsigned int ui = static_cast<unsigned int>( c );
    // std::cout << "c =" << std::hex << c << std::dec << std::endl;
    // std::cout << "& =" << std::hex << (c & 0xC0) << std::dec << std::endl;
        
    // std::cout << "ui=" << std::hex << ui << std::dec << std::endl;
    // std::cout << "& =" << std::hex << (ui & 0xC0) << std::dec << std::endl;
    // begins with 0x110.. .... ?

    // begins with 0x11110...
    if ((c & 0xF8) == 0xF8) {
      std::cerr << "NOT IMPLEMENTER UTF with 4 bytes" << std::endl;
    }
    // begins with 0x1110....
    else if ((c & 0xE0) == 0xE0) {
      std::cerr << "NOT IMPLEMENTER UTF with 3 bytes" << std::endl;
    }
    // char begins with 0x110.....
    if ((c & 0xC0) == 0xC0) {
      //std::cout << "Start of 2 bytes UTF8" << std::endl;
      utf8.push_back( c );

      code = c & 0x1F;
      //std::cout << "  code=" << std::hex << code << std::dec << "  " << code << std::endl;
      code = (code << 6); // make room for 2nd part of CodePoint
      //std::cout << "  code=" << std::hex << code << std::dec << "  " << code << std::endl;

      it += 1;
      c = *it;
      utf8.push_back( c );
      //utf8.push_back( 0 );
      //std::cout << "  c =" << std::hex << static_cast<unsigned char>(c) << std::dec << std::endl;
      //std::cout << "  2nd=" << std::hex << (c & 0x3F) << std::dec << "  " << (c & 0x3F) << std::endl; 
      code += (c & 0x3F);
    }
    else {
      utf8.push_back( c );
      //utf8.push_back( 0 );
      code = static_cast<int>( *it );
    }
    
    std::cout << "[" << code << "] " << utf8 << std::endl;
    code = 0;
    utf8.clear();
    it += 1;
  }
  
}

int main(int argc, char *argv[])
{
  std::string utf8 = std::string( "ABÉgiéç" );
  std::cout << "[" << utf8.size() << "]: " << utf8 << std::endl;

  for( auto itt = utf8.begin(); itt != utf8.end(); ++itt)  {
    char16_t c = static_cast<char16_t>( *itt );
    std::cout << *itt << " = " << c << std::endl;
  }

  print_as_char( utf8.begin(), utf8.end() );

  int codes[] = {65, 66, 201, 103, 105, 233, 231};
  for( auto& code: codes) {
    std::cout << code << " => " << str_from_code( code ) << std::endl;
  }
  
  // // conversion to utf16
  // std::wstring_convert<std::codecvt_utf8_utf16<char16_t>> converter;
  // std::wstring wstr = converter.from_bytes(utf8);

  // std::wcout << wstr << std::endl;
  
  return 0;
}
