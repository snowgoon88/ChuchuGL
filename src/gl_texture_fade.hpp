/* -*- coding: utf-8 -*- */

#ifndef GL_TEXTURE_FADE_HPP
#define GL_TEXTURE_FADE_HPP

/** 
 * Affiche des Textures sur des rectangle, en utilisant des shaders,
 * avec la possibilité de fade...
 */

#include <gl_utils.hpp>

// ***************************************************************************
// ***************************************************************** GLTexture
// ***************************************************************************
class GLTextureFade
{
public:
  // ***************************************************** GLTexture::creation
  GLTextureFade() 
  {
    // Charger les Shaders CARRE + TEXTURE
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader("src/shaders/chuchu.v.glsl", GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if((fs = GLUtils::create_shader("src/shaders/sprite_fade.f.glsl", GL_FRAGMENT_SHADER)) == 0)
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
      std::cerr << "__GLTexture: Pb avec glLinkProgam "<< std::endl;
      std::cerr << GLUtils::str_compiler_log(_program) << std::endl;
      exit( EXIT_FAILURE );
    }

    // Lier les variables des Shaders
    // coord 2D
    const char* attribute_name = "coord2d";
    _attribute_coord2d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord2d == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // la variable de textute
    attribute_name = "texcoord";
    _attribute_texcoord = glGetAttribLocation(_program, attribute_name);
    if (_attribute_texcoord == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );
    }
    // 'm'(+vp) matrix => on peut pas pop/push !!
    const char* uniform_name = "mvp";
    _uniform_mvp = glGetUniformLocation(_program, uniform_name);
    if (_uniform_mvp == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
    uniform_name = "fade";
    _uniform_fade = glGetUniformLocation(_program, uniform_name);
    if (_uniform_fade == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
  };
  // ************************************************** GLTexture::destruction
  ~GLTextureFade()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
  };
  // **************************************************** GLTexture::attributs
  const GLuint& program() const{ return _program; };
  const GLint&  attribute_coord2d() const { return _attribute_coord2d; };
  const GLint&  attribute_texcoord() const { return _attribute_texcoord; };
  const GLint&  uniform_mvp() const { return _uniform_mvp; };
  const GLint&  uniform_fade() const { return _uniform_fade; };
private:
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord2d, _attribute_texcoord;
  /** uniform var */
  GLint _uniform_mvp, _uniform_fade;
};

#endif // GL_TEXTURE_HPP
