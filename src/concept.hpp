/* -*- coding: utf-8 -*- */

#ifndef CONCEPT_HPP
#define CONCEPT_HPP

/** 
 * Les différents "concept" génériques utilisés dans le jeu.
 *
 * PAS destiné à être inclus ou compilé.
 */

namespace concept {
// ***************************************************************************
// ****************************************************************** GLScreen
// ***************************************************************************
/** 
 * Le concept de GLScreen permet d'abstraire toute application qui va
 * interagir via OpenGL et GLFW.
 * C'est en lien avec GLEngine::run( ... ).
 * 
 * voir par exemple gl_screen.hpp
 */
class GLScreen
{
public:
  // ********************************************************** GLScreen::init
  /**
   * init() sera appelée une fois avant render()
   * Généralement utilisé pour:
   *  - changer les callback pour les touches.
   *  - charger les sprites utiles.
   */
  void init() {};
  // ********************************************************* GLScreen::render
  /**
   * render() est bloquant, ne rendant la main que quand le GLSCreen 
   * est terminé.
   * Pas de surprise : gère l'affichage en utilisant OpenGL.
   */
  void render () {};
  // **************************************************** GLScreen::final_state
  /**
   * final_state()
   * Pour récupérer l'état dans lequel termine ce GLScreen une fois terminé.
   */
  bool final_state() const { return _finished; };
};
}; // namespace concept;

#endif // GL_SCREEN_HPP
