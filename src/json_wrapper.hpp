/* -*- coding: utf-8 -*- */

#ifndef JSON_WRAPPER_HPP
#define JSON_WRAPPER_HPP

/** 
 * Some Wrapper used by rapidjson over std::ostream and std::istream.
 * 
 * see : http://miloyip.github.io/rapidjson/md_doc_stream.html#FileStreams 
 */

#include <iostream>
// assert are disabled.
#define NDEBUG 
#include <assert.h>

namespace JSON
{
  // ************************************************************ OStreamWrapper
  class OStreamWrapper {
  public:
    typedef char Ch;
    OStreamWrapper(std::ostream& os) : os_(os) {
    }
    Ch Peek() const { assert(false); return '\0'; }
    Ch Take() { assert(false); return '\0'; }
    size_t Tell() const { return 0;} // not needed in fact
    Ch* PutBegin() { assert(false); return 0; }
    void Put(Ch c) { os_.put(c); } // 1
    void Flush() { os_.flush(); } // 2
    size_t PutEnd(Ch*) { assert(false); return 0; }
  private:
    OStreamWrapper(const OStreamWrapper&);
    OStreamWrapper& operator=(const OStreamWrapper&);
    std::ostream& os_;
  };
  // ************************************************************ IStreamWrapper
  class IStreamWrapper {
  public:
    typedef char Ch;
    IStreamWrapper(std::istream& is) : is_(is) {
    }
    Ch Peek() const { // 1
      int c = is_.peek();
      return c == std::char_traits<char>::eof() ? '\0' : (Ch)c;
    }
    Ch Take() { // 2
      int c = is_.get();
      return c == std::char_traits<char>::eof() ? '\0' : (Ch)c;
    }
    size_t Tell() const { return (size_t)is_.tellg(); } // 3
    Ch* PutBegin() { assert(false); return 0; }
    void Put(Ch) { assert(false); }
    void Flush() { assert(false); }
    size_t PutEnd(Ch*) { assert(false); return 0; }
  private:
    IStreamWrapper(const IStreamWrapper&);
    IStreamWrapper& operator=(const IStreamWrapper&);
    std::istream& is_;
  };
}


#endif // JSON_WRAPPER_HPP
