///////////////////////////////////////////////////////////////////////////////
// Interface for loading meshes.
///////////////////////////////////////////////////////////////////////////////

#ifndef Mesh_Included_h
#define Mesh_Included_h

#include <cstdint>
#include <iostream>

struct mesh_t
{
    mesh_t(const char* file_name);
    ~mesh_t();
    FILE* fp = 0;       // Mesh file.
    uint32_t verts = 0; // Number of vertices.
    uint32_t faces = 0; // Number of triangle faces.
    float* vpos = 0;    // Positions (3 per row).
    float* vnorm = 0;   // Normals (3 per row).
    float* vcolor = 0;  // Colors (3 per row, from 0.0 to 1.0).
    uint32_t* elem = 0; // Face elements (3 per row).
};

#endif // Mesh_Included_h
