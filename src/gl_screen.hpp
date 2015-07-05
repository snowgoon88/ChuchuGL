/* -*- coding: utf-8 -*- */

#ifndef GL_SCREEN_HPP
#define GL_SCREEN_HPP

/** 
 * Un exemple de concept::GLScreen pour gérer l'écran de 
 * départ du jeu.
 */

#include <memory>                        // std::unique_ptr

// OpenGL
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Model
// Viewer
#include <gl_texture.hpp>
#include <gl_sprite.hpp>
#include <gl_texture_fade.hpp>
#include <gl_sprite_fade.hpp>

// ******************************************************************** GLOBAL
typedef std::unique_ptr<GLSprite>  GLSpritePtr;
typedef std::unique_ptr<GLSpriteFade>  GLSpriteFadePtr;

#define ROCKET_RADIUS  1.73f
#define ROCKET_X      -0.59
#define ROCKET_Y       0.67
#define ROCKET_SPEED   150.f
#define LOGO_X        -0.42
#define LOGO_Y         0.32
#define LOGO_SCALE     1.00
#define START_X       -0.11
#define START_Y       -1.38
#define START_SCALE    1.00
// ***************************************************************************
// ****************************************************************** GLScreen
// ***************************************************************************
class GLScreen
{
public:
  // ****************************************************** GLScreen::creation
  /** Création avec une window déjà créée */
  GLScreen( GLEngine& engine ) : 
    _window(engine.window()),
    _finished(false),
    _gl_texture(engine.gl_texture()),
    _gl_texture_fade(engine.gl_texture_fade()),
    _gl_fond(nullptr), _gl_rocket(nullptr), _gl_logo(nullptr),
    _gl_start(nullptr)
  {
  };
  // *************************************************** GLScreen::destruction
  ~GLScreen()
  {
  };
  // ********************************************************** GLScreen::init
  void init()
  {
    // Installe les callback pour les touches
    glfwSetWindowUserPointer( _window, this);
    glfwSetKeyCallback(_window, key_callback);

    // Le fond d'écran comme un Sprite
    _gl_fond = GLSpritePtr(new GLSprite( _gl_texture,
					 "../Images/tex_title.png",
					 1, 1, {-3.2,-2.4}, {3.2,2.4} ) );
    _gl_rocket = GLSpritePtr(new GLSprite( _gl_texture,
					   "../Images/tex_titlerocket.png",
					   1, 1, {-0.48,-0.71}, {0.48,0.71} ));
    _gl_logo = GLSpritePtr(new GLSprite( _gl_texture,
					    "../Images/tex_title_logo.png",
					    1, 1, {-2.56,-1.32}, {2.56,1.32}));
    _gl_start = GLSpriteFadePtr(new GLSpriteFade( _gl_texture_fade,
					  "../Images/tex_pressstart.png",
					  1, 1, {-1.33,-0.16}, {1.32,0.16}));
  };
  // ********************************************************* GLScreen::render
  void render ()
  {
    // Transparence
    // Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float compteur = 0;
    while (!glfwWindowShouldClose(_window) and not _finished) {
      // update screen size
      glfwGetFramebufferSize(_window, &_screen_width, &_screen_height);
      
      // TODO Vérifier ce que fait glViewport
      glViewport(0, 0, _screen_width, _screen_height);
      /* Clear the background as black */
      glClearColor(0., 0., 0., 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

      // Pour préserver mon ratio 640x400
      float width = 6.4f;
      float height = 4.8f;
      float ratio = height / width;
      if( _screen_width * ratio > _screen_height ) {
	width = (float)_screen_width / (float)_screen_height * height;
      }
      else {
	height = (float)_screen_height / (float)_screen_width * width;
      }
      
      // Projection (to 2D screen)
      glm::mat4 projection = glm::ortho( -width/2.f, width/2.f,
					 -height/2.f, height/2.f,
					 -1.0f, 1.0f );

      // Fond
      _gl_fond->pre_render();
      _gl_fond->render( projection, {0,0}, 1.0, 0 );
      _gl_fond->post_render();

      // Rocket
      _gl_rocket->pre_render();
      // Calculer la Transformation
      float theta = (float) -M_PI/ ROCKET_SPEED * compteur;
      // translation centre
      glm::mat4 trans_c = glm::translate(glm::mat4(1.0f),
      					 glm::vec3( ROCKET_X,
      						    ROCKET_Y,
      						    0.0));
      // rotation autour du centre
      glm::mat4 rot = glm::rotate( glm::mat4(1.0f),
      				   theta,
      				   glm::vec3( 0.f, 0.f, 1.0f));
      // translation du rayon
      glm::mat4 trans_r = glm::translate(glm::mat4(1.0f),
					 glm::vec3( -ROCKET_RADIUS,
						    0.0,
						    0.0));
      glm::mat4 mvp = projection * trans_c * rot * trans_r;
      _gl_rocket->render( mvp, {0,0}, 1.0, 0 );
      _gl_rocket->post_render();
      
      // Logo
      _gl_logo->pre_render();
      _gl_logo->render( projection, {LOGO_X, LOGO_Y}, LOGO_SCALE, 0);
      _gl_logo->post_render();

      // Start
      float fade = fmod( compteur, 40.f)/20.f;
      if( fade > 1.f ) fade = 2.f - fade;
      _gl_start->pre_render();
      _gl_start->render( projection, {START_X,START_Y}, START_SCALE, 0,
			 fade );
      _gl_start->post_render();

      //std::cout << "POS={"<<_pos.x<<", "<<_pos.y<<"} scale=" << _scale << std::endl;
      //std::cout << "compteur="<<compteur<<" fade="<<fade << std::endl;

      // Remove any programm so that glText can "work"
      glUseProgram(0);

      glfwSwapBuffers(_window);
      glfwPollEvents();

      compteur += 1.0f;
    }
  }
  // *************************************************** GLScreen::final_state
  bool final_state() const { return _finished; };
  // ***************************************************** GLScreen::attributs
private:
  /** Ptr sur la Fenetre */
  GLFWwindow* _window;
  int _screen_width, _screen_height;
  /** ready */
  bool _finished;
  /** Shaders */
  const GLTexture&     _gl_texture;
  const GLTextureFade& _gl_texture_fade;
  /* Sprites */
  GLSpritePtr _gl_fond, _gl_rocket, _gl_logo;
  GLSpriteFadePtr _gl_start;
  /** Positionning */
  Vec2 _pos;
  double _scale;
  //******************************************************** GLScreen::callback
  /**
   * Callback qui gère les événements 'key'
   */
  static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    // ESC => Quit"ACRUX HAMA X-Style Pad"
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    // sinon callback de la classe
    else if( action == GLFW_PRESS ) {
      //std::cout << "key_callback = " << key << std::endl;
      ((GLScreen *)glfwGetWindowUserPointer(window))->on_key_pressed( key );
    }
  }
  // ************************************************ GLScreen::public_callback
  void on_key_pressed( int key ) 
  {
    //std::cout << "GLWindow::key_pressed key=" << key << std::endl;
    if( key == GLFW_KEY_UP) {
      _pos.y += 0.01;
    }
    else if( key == GLFW_KEY_DOWN) {
      _pos.y -= 0.01;
    }
    else if( key == GLFW_KEY_LEFT) {
      _pos.x -= 0.01;
    }
    else if( key == GLFW_KEY_RIGHT) {
      _pos.x += 0.01;
    }
    else if( key == GLFW_KEY_Q) {
      _scale -= 0.01;
    }
    else if( key == GLFW_KEY_W) {
      _scale += 0.01;
    }
    // End
    else if( key == GLFW_KEY_SPACE or key == GLFW_KEY_ENTER) {
      _finished = true;
    }
  };
  /**
   * callback pour gérer les messages d'erreur de GLFW
   */
  static void error_callback(int error, const char* description)
  {
    std::cerr <<  description << std::endl;
  }
};

#endif // GL_SCREEN_HPP
