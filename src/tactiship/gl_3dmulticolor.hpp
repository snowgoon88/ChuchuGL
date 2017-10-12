/* -*- coding: utf-8 -*- */

#ifndef GL_3DMULTICOLOR_HPP
#define GL_3DMULTICOLOR_HPP

/** 
 * Render lines and triangle of vertex with pos+rgb.
 * Has also a fade uniform variable.
 */

#include <gl_utils.hpp>
#include <string>

// ***************************************************************************
// ************************************************************ GL3DMulticolor
// ***************************************************************************
class GL3DMulticolor
{
public:
  // ************************************************ GL3DMulticolor::creation
  GL3DMulticolor( std::string shadername ) 
  {
    // Charger les Shaders LINE + TRIANGLE 3D
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader(shadername+std::string(".v.glsl"), GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if((fs = GLUtils::create_shader(shadername+std::string(".f.glsl"), GL_FRAGMENT_SHADER)) == 0)
      exit( EXIT_FAILURE );
    /** GLSL program */
    // link Vertex et Fragment Shaders
    // program est une variable GLOBALE
    _program = glCreateProgram();
    glAttachShader(_program, vs);
    glAttachShader(_program, fs);
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
      std::cerr << "__GL3DMulticolor: Pb avec glLinkProgam "<< std::endl;
      std::cerr << GLUtils::str_compiler_log(_program) << std::endl;
      exit( EXIT_FAILURE );
    }

    // Lier les variables des Shaders
    // coord 3D
    const char* attribute_name = "coord3d";
    _attribute_coord3d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord3d == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // variable 'v_color' (RGB) from GLSL program
    attribute_name = "v_color";
    _attribute_v_color = glGetAttribLocation(_program, attribute_name);
    if (_attribute_v_color == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );;
    }

    // 'm'(+vp) matrix => on peut pas pop/push !!
    const char* uniform_name = "mvp";
    _uniform_mvp = glGetUniformLocation(_program, uniform_name);
    if (_uniform_mvp == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // fade : uniform (ALPHA) 
    uniform_name = "fade";
    _uniform_fade = glGetUniformLocation(_program, uniform_name);
    if (_uniform_fade == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );;
    }
  }
  // ********************************************* GL3DMulticolor::destruction
  virtual ~GL3DMulticolor()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
  }
  // *********************************************** GL3DMulticolor::attributs
  const GLuint& program() const{ return _program; };
  const GLint&  attribute_coord3d() const { return _attribute_coord3d; };
  const GLint&  attribute_v_color() const { return _attribute_v_color; };
  const GLint&  uniform_mvp() const { return _uniform_mvp; };
  const GLint&  uniform_fade() const { return _uniform_fade; };
protected:
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord3d, _attribute_v_color;
  /** uniform var */
  GLint _uniform_mvp, _uniform_fade;
};

#endif // GL_3DMULTICOLOR_HPP
