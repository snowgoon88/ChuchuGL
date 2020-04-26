/* -*- coding: utf-8 -*- */

#ifndef GL_SHADER_HPP
#define GL_SHADER_HPP

/** 
 * Load GLSL shaders (Vertex, fragment)
 * and provide use(), get_attrib(), get_uniform()
 */

#include <gl_utils.hpp>
#include <string>
#include <sstream>

// TODO proper doc !! use snippets !
// ***************************************************************************
// ************************************************************ GL3DMulticolor
// ***************************************************************************
class GLShader
{
public:
  GLShader( const std::string& vertex_path,
            const std::string& fragment_path,
            const std::string& geometry_path="" )
  {
    // id of shaders
    GLuint vertex, fragment, geometry;
    std::string vertex_code, fragment_code, geometry_code;
    
    // get code
    vertex_code = read_code( vertex_path );
    fragment_code = read_code( fragment_path );
    if( not( geometry_path == "" ) ) {
      geometry_code = read_code( geometry_path );
    }

    // compile
    vertex = glCreateShader( GL_VERTEX_SHADER );
    compile_code( vertex, vertex_code, vertex_path );

    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    compile_code( fragment, fragment_code, fragment_path );

    if( not( geometry_path == "") ) {
      geometry = glCreateShader( GL_GEOMETRY_SHADER );
      compile_code( geometry, geometry_code, geometry_path );
    }

    // link into program
    _program = glCreateProgram();
    glAttachShader(_program, vertex);
    glAttachShader(_program, fragment);
    if( not( geometry_path == "") ) {
      glAttachShader(_program, geometry );
    }
    glLinkProgram(_program);
    // print linking error if any
    GLint link_ok = GL_FALSE;
    glGetProgramiv( _program, GL_LINK_STATUS, &link_ok);
    if ( !link_ok ) {
      std::cerr << "__ERROR::LINK" << std::endl;
      std::cerr << str_compiler_log( _program ) << std::endl;
      exit( EXIT_FAILURE );
    }
    // delete shaders as no longer necessary
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    if( not( geometry_path == "") ) {
      glDeleteShader( geometry );
    }
    
  }
  void use() 
  {
    glUseProgram( _program );
  }
  GLuint getUniformLocation( const std::string& name ) const
  {
    return glGetUniformLocation( _program, name.c_str() );
  }
  GLuint getAttribLocation( const std::string& name ) const
  {
    return glGetAttribLocation( _program, name.c_str() );
  }
    
  std::string read_code( const std::string& path )
  {
    std::string shader_code;
    std::ifstream shader_file;
    // ensure ifstream objects can throw exceptions:
    shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        shader_file.open( path );
        std::stringstream shader_stream;
        // read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();
        // close file handlers
        shader_file.close();
        // convert stream into string
        shader_code   = shader_stream.str();
    }
    catch(std::ifstream::failure e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ ";
      std::cout << path << std::endl;
      exit( EXIT_FAILURE );
    }

    return shader_code;
  }

  void compile_code( const GLuint shader,
                     const std::string& code_string,
                     const std::string& path )
  {
    const GLchar* code = code_string.c_str();
    glShaderSource( shader, 1 /* 1 string */, &code, NULL);
    glCompileShader( shader );
    
    // Print compile errors if any
    GLint compile_ok = GL_FALSE;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_ok);
    if ( !compile_ok ) {
      std::cerr << "__" << path << " : ERROR::COMPILATION" << std::endl;
      std::cerr << str_compiler_log( shader ) << std::endl;
      glDeleteShader( shader );
      exit( EXIT_FAILURE );
    }
  }
  std::string str_compiler_log( const GLuint object )
  {
      // Longueur du message
    GLint log_length = 0;
    if (glIsShader( object ))
      glGetShaderiv( object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram( object ))
      glGetProgramiv( object, GL_INFO_LOG_LENGTH, &log_length);
    else {
      return std::string("'object' n'est ni un Shader ni un Program GLSL");
    }
    
    // Copie le message dans string
    char* log = new char[log_length+1];
    if (glIsShader( object )) {
      glGetShaderInfoLog( object, log_length, NULL, log);
    }
    else if (glIsProgram( object )) {
      glGetProgramInfoLog( object, log_length, NULL, log);
    }
    log[log_length] = '\0';
    
    std::string str_log(log);
    delete[] log;
    
    return str_log;
  }
  
  // **************************************************** GLTexture::attributs
  const GLuint& program() const{ return _program; };
protected:
  /** Program GLSL */
  GLuint _program;
            
};



#endif // GL_SHADER
