/* -*- coding: utf-8 -*- */

#ifndef GL_UTILS_HPP
#define GL_UTILS_HPP

/** 
 * Fonction utiles pour faciliter OpenGL.
 *
 * 
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <fstream>                   // std::ifstream 

// ***************************************************************************
// ******************************************************************* GLUtils
// ***************************************************************************
class GLUtils 
{
public:
  // ************************************************************* create_shader
  /**
   * Compile un Shader d'après un fichier. Avec erreur de compilation.
   *
   * @param type : GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER
   * @return handle vers un SHADER compilé
   */
  static GLuint create_shader(const std::string& filename, GLenum type)
  {
#ifdef GL_ES_VERSION_2_0
    std::cout << "OpenGL ES 2.0" << std::endl;
#else
    std::cout << "OpenGL ES 2.1" << std::endl;
#endif

    read_shader(filename);
    if (_buffer == NULL) {
      std::cerr << "Erreur avec " << filename << std::endl;
      return 0;
    }
    
    // Crée le Shader
    GLuint res = glCreateShader(type);
    const GLchar* sources[2] = {
#ifdef GL_ES_VERSION_2_0
      "#version 100\n"
      "#define GLES2\n",
#else
      "#version 120\n",
#endif
      _buffer };
    // Lie avec les sources lues
    glShaderSource(res, 2, sources, NULL);
    // Compile
    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    // Si pb, on affiche les erreurs.
    if (compile_ok == GL_FALSE) {
      std::cerr << "__" << filename << " : Pb de COMPILATION" << std::endl;
      std::cerr << str_compiler_log(res) << std::endl;
      glDeleteShader(res);
      if( _buffer ) delete[] _buffer;
      _buffer = NULL;
      return 0;
    }

    if( _buffer ) delete[] _buffer;
    _buffer = NULL;
    return res;
  }
  // ********************************************************** str_compiler_log
  /** Donne des info sur la compilation de Shaders */
  static std::string str_compiler_log(GLuint object) 
  {
    // Longueur du message
    GLint log_length = 0;
    if (glIsShader(object))
      glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
      glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else {
      return std::string("'object' n'est ni un Shader ni un Program GLSL");
    }

    // Copie le message dans string
    char* log = new char[log_length+1];
    if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);
    log[log_length] = '\0';

    std::string str_log(log);
    delete[] log;

    return str_log;
  }
private:
  // *************************************************************** read_shader
  /** Charger des Shaders dans _buffer */
  static void read_shader( const std::string& filename ) 
  {
    // Stream pour lire
    std::ifstream is ( filename.c_str(), std::ifstream::binary);
    if (is) {
      // get length of file:
      is.seekg (0, is.end);
      int length = is.tellg();
      is.seekg (0, is.beg);
      
      if(_buffer) delete[] _buffer;
      _buffer = new char[length+1];
      
      std::cout << filename << " reading " << length << " characters... ";
      // read data as a block:
      is.read (_buffer,length);
      _buffer[length] = '\0';
      if (is) {
 	std::cout << "all characters read successfully." << std::endl;
	std::cout << "-----" << std::endl;
	std::cout << _buffer << std::endl;
	std::cout << "-----" << std::endl;
      }
      else {
	std::cout << "error: only " << is.gcount() << " could be read" << std::endl;
	if(_buffer) delete[] _buffer;
	_buffer = NULL;
      }
      is.close();
    }
  }
  // ***************************************************************** attributs
  /** Buffer pour lire des shaders */
  static char* _buffer;
};
char* GLUtils::_buffer = NULL;

#endif // GL_UTILS_HPP
