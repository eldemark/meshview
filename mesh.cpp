///////////////////////////////////////////////////////////////////////////////
// Interface for loading meshes.
// Mesh file format: text file with header, vertex, normal, color, element
// array, in that order, whitespace delimited rows.
// Header (row 1): [vertices] [elements]
// Each row has 3 floats except the elements which are 3 ints.
///////////////////////////////////////////////////////////////////////////////

#include "mesh.h"

#include <cstdint>
#include <iostream>

namespace
{

float* load_float_data(FILE* fp, uint32_t n)
{
    float* data = (float*)malloc(n * sizeof(float) * 3);

    if (data == NULL)
    {
        perror("load_float_data()");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < n; i++)
    {
        float x = 0;
        float y = 0;
        float z = 0;
        int items = fscanf(fp, "%10f%10f%10f", &x, &y, &z);

        if (items != 3 || items == EOF)
        {
            fprintf(stderr, "Mesh file read error.\n");
            exit(EXIT_FAILURE);
        }

        uint32_t stride = i * 3;
        data[stride + 0] = x;
        data[stride + 1] = y;
        data[stride + 2] = z;
    }

    return data;
}

} // unnamed namespace


// Return mesh loaded from given file.
mesh_t::mesh_t(const char* file_name)
{
    // Open file for reading.
    fp = fopen(file_name, "r");

    if (fp == NULL)
    {
        perror("mesh_alloc()");
        exit(EXIT_FAILURE);
    }

    // Read header init mesh.
    // First line is number of verts and triangles.
    int items = fscanf(fp, "%6u%6u", &verts, &faces);

    if (items == EOF)
    {
        fprintf(stderr, "Mesh file read error.\n");
        exit(EXIT_FAILURE);
    }

    // Order of float arrays: position, normals, colors.
    vpos = load_float_data(fp, verts);
    vnorm = load_float_data(fp, verts);
    vcolor = load_float_data(fp, verts);

    // Load face elements.
    elem = (uint32_t*)malloc(faces * sizeof(float) * 3);

    if (elem == NULL)
    {
        perror("mesh_alloc()");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < faces; i++)
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
        int items = fscanf(fp, "%10d%10d%10d", &x, &y, &z);

        if (items != 3 || items == EOF)
        {
            fprintf(stderr, "Mesh file read error.\n");
            exit(EXIT_FAILURE);
        }

        uint32_t stride = i * 3;
        elem[stride + 0] = x;
        elem[stride + 1] = y;
        elem[stride + 2] = z;
    }
}

mesh_t::~mesh_t()
{
    free(vpos);
    free(vnorm);
    free(vcolor);
    free(elem);
    fclose(fp);
}
