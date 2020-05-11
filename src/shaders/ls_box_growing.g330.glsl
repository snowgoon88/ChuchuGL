#version 330 core

// Given Line_Stripes (so 4 points in), compute a box around the current
// line segment.
// Receive points in 3Dspace, compute box in that space and then give
// screen coordinates (NDC) points to the Shader

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices=32) out;
//layout (line_strip, max_vertices=32) out;

// in vec3 pos3[];           // vertex position in 3Dspace
// in vec3 norm3[];          // vertex normal in 3Dspace
// in vec3 col3[];

in VTX_NOR {
  vec3 pos3;
  vec3 norm3;
} gs_vtx_in[];
in COL {
  vec3 col3;
} gs_col_in[];

out vec2 end_segment[2];  // start and end of current segment, screen
out vec2 pos2;            // position of point, screen
out vec4 dbg_color;

// Usual transforms
uniform mat4  u_view_model4x4 = mat4(1.f);
uniform mat4  u_proj4x4;
uniform float u_radius;

vec4 bbox[8]; // bounding box around segment, 3dSpace

vec2 to_screen_coord(vec3 pt)
{
  // Apply PVM; making sure that w stays to 1
  vec3 v = (u_view_model4x4 * vec4( pt, 1)).xyz;
  vec4 u = u_proj4x4 * vec4( v, 1);
  // divide by w to get NDC coordinates, as Fragment worj with that
  return u.xy / u.w;
}

void emit( vec4 vertex )
{
  // to screen coord for the Fragment
  gl_Position = vertex;
  pos2 = vertex.xy / vertex.w;
  EmitVertex();
}
void emit_face( int a, int b, int c, int d )
{
  // gl_Position is default output of GeometryS
  emit( bbox[a]);
  emit( bbox[b] );
  emit( bbox[c] );
  emit( bbox[d] );
  EndPrimitive();
}

void emit_star( vec3 vtx, vec3 color )
{
  dbg_color = vec4( color,1 );
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(0.2,0,0), 1.0);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(-0.2,0,0), 1.0);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(0.,0.2,0), 1.0);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(0,-0.2,0), 1.0);
  EmitVertex();
  EndPrimitive();
}
void emit_small_star( vec3 vtx, vec3 color )
{
  dbg_color = vec4( color,1 );
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(0.1,0.1,0), 1.0);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(0.1,-0.1,0), 1.0);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(-0.1,-0.1,0), 1.0);
  EmitVertex();
  EndPrimitive();

  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx+vec3(-0.1,0.1,0), 1.0);
  EmitVertex();
  EndPrimitive();
}


// for each vertex X, draw a small segment X,X+(0.1,0,0)
void draw_segment( vec3 vtx1, vec3 vtx2 )
{
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx1, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx1+vec3(0.1,0,0), 1.0);
  EmitVertex();
  EndPrimitive();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx1, 1.0);
  EmitVertex();
  gl_Position = u_proj4x4 * u_view_model4x4 * vec4( vtx2+vec3(-0.1,0,0), 1.0);
  EmitVertex();
  EndPrimitive();
}

void main()
{
  // The current segment
  // as we have line_stripe_adjacency, start is the input 1 and end is input 2
  vec3 start_seg = gs_vtx_in[1].pos3;
  vec3 end_seg = gs_vtx_in[2].pos3;
  
  // Give the segment, in screen coordinate, to Fragment
  end_segment[0] = to_screen_coord( start_seg );
  end_segment[1] = to_screen_coord( end_seg );

  //draw_segment( pos3[1], pos3[2] );
  //emit_star( pos3[1], norm3[1] );
  //emit_small_star( pos3[2], norm3[2] );
  // emit_star( pos3[1], vec3(0,1,0) );
  // emit_star( pos3[2], vec3(0,0,1) );
  // emit_star( pos3[3], vec3(1,0,1) );
  
  // To build the faces of the BoundingBox, need some vectors
  vec3 v01 = normalize( gs_vtx_in[1].pos3 - gs_vtx_in[0].pos3 );
  vec3 v12 = normalize( gs_vtx_in[2].pos3 - gs_vtx_in[1].pos3 );
  vec3 v23 = normalize( gs_vtx_in[3].pos3 - gs_vtx_in[2].pos3 );
  vec3 dir1 = normalize( v01 + v12 );       // mean dir of line from 1
  vec3 dir2 = normalize( v12 + v23 );       // mean dir of line from 2

  // Compute the corners of the BoundingBox
  vec3 dir, normal, perp;
  float radius = u_radius;
  
  dir = dir1; normal = gs_vtx_in[1].norm3; perp = cross( dir, normal );
  bbox[0] = u_proj4x4 * u_view_model4x4 * vec4( start_seg + normal*radius + perp*radius, 1);
  bbox[1] = u_proj4x4 * u_view_model4x4 * vec4( start_seg + normal*radius - perp*radius, 1);
  bbox[2] = u_proj4x4 * u_view_model4x4 * vec4( start_seg - normal*radius - perp*radius, 1);
  bbox[3] = u_proj4x4 * u_view_model4x4 * vec4( start_seg - normal*radius + perp*radius, 1);

  dir = dir2; normal = gs_vtx_in[2].norm3; perp = cross( dir, normal );
  bbox[4] = u_proj4x4 * u_view_model4x4 * vec4( end_seg + normal*radius + perp*radius, 1);
  bbox[5] = u_proj4x4 * u_view_model4x4 * vec4( end_seg + normal*radius - perp*radius, 1);
  bbox[6] = u_proj4x4 * u_view_model4x4 * vec4( end_seg - normal*radius - perp*radius, 1);
  bbox[7] = u_proj4x4 * u_view_model4x4 * vec4( end_seg - normal*radius + perp*radius, 1);

  // Now emit the 6 faces of the BoundingBox (prismoid)
  dbg_color = vec4( gs_col_in[1].col3, 1);
  emit_face(0,1,3,2);
  emit_face(5,4,6,7);
  emit_face(4,5,0,1);
  emit_face(3,2,7,6);
  emit_face(0,3,4,7);
  emit_face(2,1,6,5);
}

