/* -*- coding: utf-8 -*- */

#ifndef GL3DSHIP_HPP
#define GL3DSHIP_HPP

/** 
 * Affiche un vaisseau en 2 couleurs.
 */

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>           // glm::quat 
#include <glm/gtx/quaternion.hpp>           // glm::quat toMat4

// ***************************************************************************
// ****************************************************************** GL3DShip
// ***************************************************************************
class GL3DShip
{
public:
  // ****************************************************** GL3DShip::creation
  GL3DShip()
  {
    // VBO avec 3d coord et rgb color pour les points du Ship
    GLfloat ship_vtx[] = {
      -1, 0, 1,         0.2,0.2,1.0, // A - b
      0, -2, 0,         0.2,0.2,1.0, // B - b
      4, 0, 0,          0.2,0.2,1.0, // C - b
      0, 2, 0,          0.2,0.2,1.0, // D - b
      -0.8,0,-0.8,      0.2,0.2,1.0, // E - b
      -0.8,0,-0.8,      1.0,0.2,0.2, // E - r
      0, -2, 0,         1.0,0.2,0.2, // B - r
      -1, 0, 1,         1.0,0.2,0.2, // A - r
      0, 2, 0,          1.0,0.2,0.2,  // D - r
      -1, 0, 2.0,       0.2,0.2,1.0, // F - b
      -0.9,-0.1,0.8,    0.2,0.2,1.0, // G - b
      0,0.0,0.8,        0.2,0.2,1.0, // H - b
      -0.9,0.1,0.8,     0.2,0.2,1.0, // I - b
    };
    //_vbo_ship_size = 5;
    // Un VBO
    glGenBuffers(1, &_vbo_ship);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_ship);
    // Pousse les points dans le VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof( ship_vtx ),
  		 ship_vtx, GL_STATIC_DRAW);
    // Qu'on peut rendre inactif avec
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Les index qui spécifieront les faces
    GLushort ship_ets[] = {
      0, 1, 2, // ABC - b
      0, 2, 3, // ACD - b
      4, 3, 2, // EDC - b
      4, 2, 1, // ECB - b
      5, 6, 7, // EBA - r
      5, 7, 8, // EAD - r
      9,10,11, // FGH - b
      9,11,12, // FHI - b
      9,12,10  // FIG - b
    };
    glGenBuffers(1, &_ibo_ship_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ship_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ship_ets),
  		 ship_ets, GL_STATIC_DRAW);
    // Optionel ?
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Charger les Shaders : LINE + TRIANGLE 3D, avec couleur
    GLint link_ok = GL_FALSE;
    GLuint vs, fs;
    /** Vertex Shader */
    if ((vs = GLUtils::create_shader("src/shaders/line_tri_xyz_rgb_fade.v.glsl",
  				     GL_VERTEX_SHADER))   == 0)
      exit( EXIT_FAILURE );
    /** Fragment Shader */
    if ((fs = GLUtils::create_shader("src/shaders/line_tri_xyz_rgb_fade.f.glsl",
  				     GL_FRAGMENT_SHADER)) == 0)
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
      std::cerr << "__GL3DShip: Pb avec glLinkProgam "<< std::endl;
      std::cerr << GLUtils::str_compiler_log(_program) << std::endl;
      exit( EXIT_FAILURE );
    }

    // Lier les variables des shaders
    // coord3d : coordonnées du vertex
    const char* attribute_name = "coord3d";
    _attribute_coord3d= glGetAttribLocation(_program, attribute_name);
    if (_attribute_coord3d == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );;
    }
    // la variable 'v_color' (RGB) du programme GLSL
    attribute_name = "v_color";
    _attribute_v_color = glGetAttribLocation(_program, attribute_name);
    if (_attribute_v_color == -1) {
      std::cerr <<  "Pb pour lier l'attribut " << attribute_name << std::endl;
      exit( EXIT_FAILURE );;
    }
    // la variable uniform 'fade' (ALPHA) du programme GLSL
    const char* uniform_name = "fade";
    _uniform_fade = glGetUniformLocation(_program, uniform_name);
    if (_uniform_fade == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );;
    }
    uniform_name = "mvp";
    _uniform_mvp = glGetUniformLocation(_program, uniform_name);
    if (_uniform_mvp == -1) {
      std::cerr <<  "Pb pour lier l'uniform " << uniform_name << std::endl;
      exit( EXIT_FAILURE );
    }
  };
  // *************************************************** GL3DShip::destruction
  virtual ~GL3DShip()
  {
    // Détruit le programme GLSL
    glDeleteProgram(_program);
    // Et les vbo, ibo
    glDeleteBuffers(1, &_vbo_ship);
    glDeleteBuffers(1, &_ibo_ship_elements);
  };
  // ******************************************************** GL3DShip::render
  void render( const glm::mat4& projection,
  	       const glm::vec3& origin = {0,0,0},
	       const glm::quat& rotation = {0,0,0,1}) const
  {
    glPushAttrib (GL_ENABLE_BIT);
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glEnable (GL_LINE_SMOOTH);

    glUseProgram( _program );
    // Ajouter le changement de position et d'orientation
    // Rotation
    glm::mat4 rotation_mtx = glm::toMat4( rotation );

    // Translation
    glm::mat4 translation_mtx = glm::translate(  glm::mat4(1.0f),
						 origin );
    glm::mat4 mvp = projection * translation_mtx * rotation_mtx;
    glUniformMatrix4fv(_uniform_mvp, 1, GL_FALSE,
     		       glm::value_ptr(mvp));
    // Fade
    glUniform1f( _uniform_fade, 1.0 );
    
    // Ship
    glBindBuffer( GL_ARRAY_BUFFER, _vbo_ship );
    glEnableVertexAttribArray(_attribute_coord3d);
    /* Describe Vertices Array to OpenGL */
    glVertexAttribPointer(
      _attribute_coord3d, // attribute
      3,                 // number of elements per vertex, here (x,y,z)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      6*sizeof(GLfloat), // stride -> car XYZ + RGB
      0                  // offset of first element
  			  );	   
    // Color
    glEnableVertexAttribArray(_attribute_v_color);
    glVertexAttribPointer(
      _attribute_v_color, // attribute
      3,                 // number of elements per vertex, here (r,g,b)
      GL_FLOAT,          // the type of each element
      GL_FALSE,          // take our values as-is
      6*sizeof(GLfloat), // stride -> car XYZ + RGB
      (GLvoid*) (3*sizeof(GLfloat)) // offset = 3 car commence par X,Y,Z
			  );
    
    /* Push each element in buffer_vertices to the vertex shader
     * according to index */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo_ship_elements);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    // std::cout << "Render ship avec " << size/3 << " triangles" << std::endl;
    glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    //glDisableVertexAttribArray(_attribute_coord3d);
    //glDisableVertexAttribArray(_attribute_v_color);

    glPopAttrib();
  };
  // ***************************************************** GL3DShip::attributs
private:
  /** Program GLSL */
  GLuint _program;
  /** Variables globale du Programme GLSL */
  GLint _attribute_coord3d, _attribute_v_color;
  /** Uniform var */
  GLint _uniform_fade;
  GLint _uniform_mvp;
  /** Vertex Buffer Objects */
  GLuint _vbo_ship;
  GLuint _ibo_ship_elements;
  
};
#endif // GL3DSHIP_HPP
