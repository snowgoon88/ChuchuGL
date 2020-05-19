/* -*- coding: utf-8 -*- */

#ifndef GL3D_ICOSPHERE_HPP
#define GL3D_ICOSPHERE_HPP

/** 
 * Builds a VBO with XYZ + ST + NOR =>     isocahedron_face_vbo()
 * - default 20 faces => 20 * (3+2+3) GLfloat
 *
 * Builds a VBO with INDICES for lines =>  isocahedron_line_vbo()
 * - default 20 faces => 20 * 3 * 2 indices
 */

class GL3DIcosphere
{
  using VTX = GLfloat;
  using VTXvec = std::vector<VTX>;
  using IDX = GLuint;
  using IDXvec = std::vector<IDX>;
public:
  // ************************************************** Icosphere::constructor
  GL3DIcosphere( bool verb=false) :
    _ico_vtx( 12*3 )
  {
    build_icosahedron_vtx( verb );
    build_icosahedron_faces( verb );

    // Make VBO for faces
    glGenBuffers(1, &_icosahedron_face_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _icosahedron_face_vbo);
    // push xyz+st+nor into VBO
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof( GLfloat ) * _icofaces_xyz_st_nor.size(),
  		 _icofaces_xyz_st_nor.data(), GL_STATIC_DRAW);
    if (verb) {
      std::cout << "  pushed " <<  _icofaces_xyz_st_nor.size() << " data into _icosahedron_face_vbo" << std::endl;
    }

    // Build VBO for line indices
    glGenBuffers(1, &_icosahedron_line_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _icosahedron_line_vbo );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint) * _icolines_idx.size(),
  		 _icolines_idx.data(), GL_STATIC_DRAW);
    if (verb) {
      std::cout << "  pushed " <<  _icolines_idx.size() << " data into _icosahedron_line_vbo" << std::endl;
    }
  }
  virtual ~GL3DIcosphere()
  {
    glDeleteBuffers( 1, &_icosahedron_face_vbo );
    glDeleteBuffers( 1, &_icosahedron_line_vbo );
  }
  
  // ************************************************ GL3DIcosphere::attributs
  const GLuint icosahedron_face_vbo() const { return _icosahedron_face_vbo; }  
  const GLuint icosahedron_line_vbo() const { return _icosahedron_line_vbo; }
  
protected:
  VTXvec _ico_vtx;               // Icosahedron vertices (12*3)
  VTXvec _icofaces_xyz_st_nor;   // Icosahedron faces (20 * 3 * [3+2+3])
  IDXvec _icolines_idx;          // Icosahedron lines (30 * 2)

  GLuint _icosahedron_face_vbo, _icosahedron_line_vbo;
  
  const float V_ANGLE = atanf( 0.5f );
  const float H_ANGLE = 2.0 * M_PI / 5.0;
protected:
  // ***************************************************** Icosphere::building
  void build_icosahedron_vtx( bool verb=false )
  {
    if (verb) {
      std::cout << "__GL3DIcosphere::build_icosahedron_vtx" << std::endl;
    }
    
    unsigned int idx = 0;
    
    // first : top at (0,0,1)
    _ico_vtx[idx*3+0] = 0;
    _ico_vtx[idx*3+1] = 0;
    _ico_vtx[idx*3+2] = 1;
    idx++;
    float elev = sinf( V_ANGLE );
    float xy_length = cosf( V_ANGLE );
    // first row at (x,y, sinf( V_ANGLE) )
    float angle = 0;
    for( unsigned int i = 0; i < 5; ++i) {
      _ico_vtx[idx*3+0] = xy_length * cosf( angle );
      _ico_vtx[idx*3+1] = xy_length * sinf( angle );
      _ico_vtx[idx*3+2] = elev;
      idx++;
      angle += H_ANGLE;
    }
    // second row at (x,y, -sinf( V_ANGLE)), starting at angle=H_ANGLE/2
    angle = H_ANGLE / 2.0;
    for( unsigned int i = 0; i < 5; ++i) {
      _ico_vtx[idx*3+0] = xy_length * cosf( angle );
      _ico_vtx[idx*3+1] = xy_length * sinf( angle );
      _ico_vtx[idx*3+2] = -elev;
      idx++;
      angle += H_ANGLE;
    }
    
    // last : bottom at (0,0,-1)
    _ico_vtx[idx*3+0] = 0;
    _ico_vtx[idx*3+1] = 0;
    _ico_vtx[idx*3+2] = -1;
    idx++;

    if (verb) {
      for( unsigned int i = 0; i < 12; ++i) {
        std::cout << "vtx[" << i << "] = ";
        std::cout << std::setprecision(2) << _ico_vtx[i*3+0] << ", ";
        std::cout << std::setprecision(2) << _ico_vtx[i*3+1] << ", ";
        std::cout << std::setprecision(2) << _ico_vtx[i*3+2];
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }
  }
  void build_icosahedron_faces( bool verb=false )
  {
    if (verb) {
      std::cout << "__GL3DIcosphere::build_icosahedron_faces" << std::endl;
    }

    // Make the tour of the top row and add 4 faces vtx
    // And 6 lines
    /**   /\  1         /  
         /__\          /__ 
        2\  /\         \  /
          \/__\3        \/__
           \  /          \ 
            \/4           \
    */
    /**
    NOTE: The texture coords are offset in order to align coords to image pixels
    Image width = 11S, height = 3T : S=1/11, T=1/3
    But in 2048x1024 image with triangles touching "up" we have
    2S = 372 / 2048 and T= 322 / 1024 
    //    (S,0)  3S  5S  7S  (9S,0)
    //       /\  /\  /\  /\  /\             //
    //      /__\/__\/__\/__\/__\(10S,T)     //
    // (0,T)\  /\  /\  /\  /\  /\           //
    //       \/__\/__\/__\/__\/__\(11S,2T)  //
    //  (S,2T)\  /\  /\  /\  /\  /          //
    //         \/  \/  \/  \/  \/           //
    //    (2S,3T)  4S  6S  8S  (10S,3T)
    */
    const float T = 322.f / 1024.f;
    const float S = 186.f / 2048.f;

    //unsigned int idx = 0; 
    //unsigned int idx_face=0;
    
    unsigned int i_top = 0;
    unsigned int i_bottom = 11;
    for( unsigned int i = 1; i <= 5; ++i) {
      unsigned int i_next = i+1;
      unsigned int i_below=i+5;
      unsigned int i_below_next=i_below+1;
      if (i==5) {
        i_next = 1;
        i_below_next = 6; 
      }

       // Top face
      add_texface( _icofaces_xyz_st_nor,
                   _ico_vtx, {i, i_next, i_top},
                   {{(i-1)*2*S,T},{i*2*S,T},{((i-1)*2+1)*S,0}}, verb);
      // Side Face 2
      add_texface( _icofaces_xyz_st_nor,
                   _ico_vtx, {i, i_below, i_next},
                   {{(i-1)*2*S,T},{((i-1)*2+1)*S,2*T},{i*2*S,T}}, verb );
      // Side Face 3
      add_texface( _icofaces_xyz_st_nor,
                   _ico_vtx, {i_next, i_below, i_below_next},
                   {{i*2*S,T},{((i-1)*2+1)*S,2*T},{((i-1)*2+3)*S,2*T}}, verb );
      // Bottom face
      add_texface( _icofaces_xyz_st_nor,
                   _ico_vtx, {i_below, i_bottom, i_below_next},
                   {{((i-1)*2+1)*S,2*T},{i*2*S,3*T},{((i-1)*2+3)*S,2*T}}, verb );

      // Lines (needs index of points in face_vtx, NOT ico_vtx)
      unsigned int idx_i = (i-1)*12;
      unsigned int idx_top = idx_i+2;
      unsigned int idx_next = idx_i+1;
      unsigned int idx_below = idx_i+4;
      unsigned int idx_below_next = idx_below+4;
      unsigned int idx_bottom = idx_below+6;
      // top to point
      _icolines_idx.push_back( idx_top );
      _icolines_idx.push_back( idx_i );
      // point to next in row
      _icolines_idx.push_back( idx_i );
      _icolines_idx.push_back( idx_next );
      // point to below
      _icolines_idx.push_back( idx_i );
      _icolines_idx.push_back( idx_below );
      // below to next in row
      _icolines_idx.push_back( idx_below );
      _icolines_idx.push_back( idx_next );
      // below to next below
      _icolines_idx.push_back( idx_below );
      _icolines_idx.push_back( idx_below_next );
      // below to bottom
      _icolines_idx.push_back( idx_below );
      _icolines_idx.push_back( idx_bottom );
    }
  }
  VTXvec compute_normal( VTXvec& src,  const std::vector<unsigned int>& idx )
  {
    VTXvec normal = {0.f, 0.f, 0.f };
    
    GLfloat ux = src[idx[1]*3+0]-src[idx[0]*3+0];
    GLfloat uy = src[idx[1]*3+1]-src[idx[0]*3+1];
    GLfloat uz = src[idx[1]*3+2]-src[idx[0]*3+2];

    GLfloat vx = src[idx[2]*3+0]-src[idx[0]*3+0];
    GLfloat vy = src[idx[2]*3+1]-src[idx[0]*3+1];
    GLfloat vz = src[idx[2]*3+2]-src[idx[0]*3+2];

    GLfloat nx = uy * vz - uz * vy;
    GLfloat ny = uz * vx - ux * vz;
    GLfloat nz = ux * vy - uy * vx;

    GLfloat length = sqrtf(nx * nx + ny * ny + nz * nz);
    if(length > EPSILON)
    {
      // normalize
      normal[0] = nx / length;
      normal[1] = ny  / length;
      normal[2] = nz / length;
    }

    return normal;
  }
  void add_texface( VTXvec& dest, VTXvec& src,
                    const std::vector<unsigned int>& idx,
                    const std::vector<VTXvec>& coord,
                    bool verb=false )
  {
    // compute normal
    VTXvec normal = compute_normal( src, idx );
    
    if (verb) {
      std::cout << "add_texface ";
      std::cout << " : (" << idx[0] << ", " << idx[1] << ", " << idx[2];
      std::cout << ")";
      std::cout << "  coord = { ";
      std::cout << "( " << coord[0][0] << ", " << coord[0][1] << "), ";
      std::cout << "( " << coord[1][0] << ", " << coord[1][1] << "), ";
      std::cout << "( " << coord[2][0] << ", " << coord[2][1] << ") ";
      std::cout << "}" << std::endl;
      std::cout << "             : n= (" <<  normal[0] << ", " << normal[1] << ", " << normal[2] << ")" << std::endl;
    }

     // Vertices
    unsigned int idx_coord = 0;
    for( auto& i: idx) {
      
      dest.push_back( src[i*3+0] );
      dest.push_back( src[i*3+1] );
      dest.push_back( src[i*3+2] );

      // TexCoord
      dest.push_back( (float) coord[idx_coord][0] );
      dest.push_back( (float) coord[idx_coord][1] );
      idx_coord++;

      // Normal
      dest.push_back( normal[0] );
      dest.push_back( normal[1] );
      dest.push_back( normal[2] );
    }
  }
}; // GL3DIcosphere


#endif // GL3D_ICOSPHERE_HPP
