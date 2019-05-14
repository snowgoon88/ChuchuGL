// Test converstion from wchar to various utf variants

#include <codecvt>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <iostream>
#include <locale>
#include <clocale>
#include <vector>

using std::cout;
using std::endl;
using std::exit;
using std::memcmp;
using std::size_t;

using std::wcout;

#if _WIN32 || _WIN64
// Windows needs a little non-standard magic for this to work.
#include <io.h>
#include <fcntl.h>
#include <locale.h>
#endif

using std::size_t;

void init_locale(void)
// Does magic so that wcout can work.
{
#if _WIN32 || _WIN64
  // Windows needs a little non-standard magic.
  constexpr char cp_utf16le[] = ".1200";
  setlocale( LC_ALL, cp_utf16le );
  _setmode( _fileno(stdout), _O_U16TEXT );
#else
  // The correct locale name may vary by OS, e.g., "en_US.utf8".
  constexpr char locale_name[] = "";
  std::locale::global(std::locale(locale_name));
  std::wcout.imbue(std::locale());
#endif
}

int main(void)
{
  // print some utf8 normally
  std::string sutf8 = std::string( "ABÉgiéç" );
  std::cout << "UTF8 = " << sutf8 << std::endl;
  for (auto it = sutf8.begin(); it != sutf8.end(); ++it) {
    int code = static_cast<int>( *it );
    std::cout << "c=" << *it;
    std::cout << "\tcode=" << code;
    std::cout << " (" << std::hex << code << std::dec << ")";
    std::cout << "\tc+=" << (*it)+256 << std::endl;
  }
  // Not working => canot cout u16string
  std::u16string sutf16 = std::u16string( u"ABÉgiéç" );
  std::cout << u"UTF16 = " << std::endl; // << sutf16 << std::endl;
  for (auto it = sutf16.begin(); it != sutf16.end(); ++it) {
    int code = static_cast<int>( *it );
    std::cout << "c=" << *it;
    std::cout << "\tcode=" << code;
    std::cout << " (" << std::hex << code << std::dec << ")" << std::endl;
  }
  std::wstring sw = std::wstring( L"ABÉgiéç" );
  std::wcout << L"SW = " << sw << std::endl;
  for (auto it = sw.begin(); it != sw.end(); ++it) {
    int code = static_cast<int>( *it );
    std::wcout << "c=" << *it;
    std::wcout << "\tcode=" << code;
    std::wcout << " (" << std::hex << code << std::dec << ")" << std::endl;
  }

  
  constexpr char16_t msg_utf16[] = u"¡Hola, mundo! \U0001F600"; // Shouldn't assume endianness.
  constexpr wchar_t msg_w[] = L"¡Hola, mundo! \U0001F600";
  constexpr char32_t msg_utf32[] = U"¡Hola, mundo! \U0001F600";
  constexpr char msg_utf8[] = u8"¡Hola, mundo! \U0001F600";

  init_locale();

  // const std::codecvt_utf16<wchar_t, 0x1FFFF, std::little_endian> converter_w;
  // const size_t max_len = sizeof(msg_utf16);
  // std::vector<char> out(max_len);
  // std::mbstate_t state;
  // const wchar_t* from_w = nullptr;
  // char* to_next = nullptr;

  // converter_w.out( state, msg_w, msg_w+sizeof(msg_w)/sizeof(wchar_t), from_w, out.data(), out.data() + out.size(), to_next );


  // if (memcmp( msg_utf8, out.data(), sizeof(msg_utf8) ) == 0 ) {
  //   wcout << L"std::codecvt_utf16<wchar_t> converts to UTF-8, not UTF-16!" << endl;
  // } else if ( memcmp( msg_utf16, out.data(), max_len ) != 0 ) {
  //   wcout << L"std::codecvt_utf16<wchar_t> conversion not equal!" << endl;
  // } else {
  //   wcout << L"std::codecvt_utf16<wchar_t> conversion is correct." << endl;
  // }
  // out.clear();
  // out.resize(max_len);

  // const std::codecvt_utf16<char32_t, 0x1FFFF, std::little_endian> converter_u32;
  // const char32_t* from_u32 = nullptr;
  // converter_u32.out( state, msg_utf32, msg_utf32+sizeof(msg_utf32)/sizeof(char32_t), from_u32, out.data(), out.data() + out.size(), to_next );

  // if ( memcmp( msg_utf16, out.data(), max_len ) != 0 ) {
  //   wcout << L"std::codecvt_utf16<char32_t> conversion not equal!" << endl;
  // } else {
  //   wcout << L"std::codecvt_utf16<char32_t> conversion is correct." << endl;
  //}

  wcout << msg_w << endl;
  return EXIT_SUCCESS;
}
