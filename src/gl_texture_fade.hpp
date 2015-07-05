/* -*- coding: utf-8 -*- */

#ifndef GL_TEXTURE_FADE_HPP
#define GL_TEXTURE_FADE_HPP

/** 
 * Affiche des Textures sur des rectangle, en utilisant des shaders,
 * avec la possibilité de fade...
 */
#include <gl_texture.hpp>

// ***************************************************************************
// ************************************************************* GLTextureFade
// ***************************************************************************
class GLTextureFade : public GLTexture
{
public:
  // ************************************************* GLTextureFade::creation
  GLTextureFade( std::string shadername ) : GLTexture( shadername ) 
  {
    const char* uniform_name = "fade";
    _uniform_fade = glGetUniformLocation(_program, uniform_name);
    if (_uniform_fade == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
  };
  // ************************************************ GLTextureFade::attributs
  const GLint&  uniform_fade() const { return _uniform_fade; };
protected:
  /** uniform var */
  GLint _uniform_fade;
};

#endif // GL_TEXTURE_FADE_HPP
