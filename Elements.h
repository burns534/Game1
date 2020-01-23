//
//  Header.h
//  Game1
//
//  Created by Kyle Burns on 1/22/20.
//  Copyright © 2020 Kyle Burns. All rights reserved.
//

#ifndef Header_h
#define Header_h
#include "NoiseMap.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
std::ofstream outfile;
struct vec3
{
    float x,y,z;
};

class Elements
{
public:
    unsigned* indices;
    float* vertices;
    unsigned vertices_size;
    NoiseMap* nmap;
    unsigned triangle_number;
    vec3* verts;
    float * normals;
    unsigned normal_number;
    float * randoms;
    Elements(int width, int height)
    {
        outfile.open("log.txt");
        vertices = new float[3 * width * height]; // contains 12 byte objects storing (x,y,z) floats
        verts = new vec3[width * height];
        nmap = new NoiseMap(width, height);
        randoms = new float[3 * width * height];
        triangle_number = 2 * ( width - 1 ) * ( height - 1 );
        vertices_size = 3 * width * height * 4; // floats are 4 bytes
        // fill vertices with appropriate data
        unsigned index = 0;
        for ( int y = 0; y < height; y++)
            for ( int x = 0; x < width; x++, index += 3)
            {
                // need to put in device coords
                vertices[index] = (2.0f * x / (float)(width - 1)) - 1.0f;
                verts[y * width + x].x = vertices[index]; // to make normal calculations simpler
                // already clamped to [0,1]
                vertices[index + 1] = nmap->noisemap[y*width + x]; // perlin z values
                if (vertices[index + 1] < 0.5f)
                vertices[index + 1] = 0.4975f + (0.0025f * nmap->n->OctavePerlin( x * 50.34f, y * 50.34f, 0, 8, .50, .003f));
                verts[y * width + x].y = vertices[index + 1];
                //outfile << "(float (2.0f * y) / (height - 1) - 1.0f:" << (float) ( 2.0f * y) / (height - 1) - 1.0f << "\n";
                vertices[index + 2] = static_cast<float> (2.0f * y) / (height - 1) - 1.0f;
                verts[y * width + x].z = vertices[index + 2];
                
                // clamp to [0,1);
                randoms[index] = (float)(rand() % 2048) / 2048;
                randoms[index + 1] = (float)(rand() % 2048) / 2048;
                randoms[index + 2] = (float)(rand() % 2048) / 2048;
                //std::cout << randoms[index] << "\n";
            }
        indices = new unsigned[6 * (width-1) * (height-1)]; // 3 * 2 floats per "tile", w-1 * h-1 "tiles""
        index = 0;
        for ( unsigned y = 0; y < height - 1; y++)
            for ( unsigned x = 0; x < width - 1; x++, index += 6) // to move through indices after each 2 triangles are added
            {
                // must make sure both triangle are generated clockwise for culling later
                // triangle 1, clockwise topleft->bottomright->bottomleft
                indices[index] = y * width + x;
                indices[index + 1] = (y + 1) * width + x + 1;
                indices[index + 2] = (y + 1) * width + x;
                // triangle 2, also clockwise bottomright->topleft->topright
                indices[index + 3] = (y + 1) * width + x + 1;
                indices[index + 4] = y * width + x;
                indices[index + 5] = y * width + x + 1;
            }
        glm::vec3 right, left, up, down, down_right, up_left;
        bool r, l, u, d, dr, ul;
        glm::vec3 norm1, norm2, norm3, norm4, norm5, norm6;
        index = 0;
        normals = new float[3 * width * height];
        
        /* FIX calculate normals for each vector... and store it in normals array
         this is done by summing the cross product of surrouding face junctions for each vector...
         going to have poor edge conditions I imagine */
        for ( int y = 0; y < height; y++ )
            for ( int x = 0; x < width; x++, index += 3)
            {
                // try to make vector to each possible directipn
                u = d = r = l = ul = dr = false;
                if ( y - 1 >= 0 && x - 1 < width )
                {
                    up_left = glm::vec3(
                                        verts[(y-1) * width + x - 1].x - verts[y * width + x].x,
                                        verts[(y-1) * width + x - 1].y - verts[y * width + x].y,
                                        verts[(y-1) * width + x - 1].z - verts[y * width + x].z
                                        );
                    ul = true;
                }
                if ( y - 1 >= 0 )
                {
                    up = glm::vec3(
                                   verts[(y-1) * width + x].x - verts[y * width + x].x,
                                   verts[(y-1) * width + x].y - verts[y * width + x].y,
                                   verts[(y-1) * width + x].z - verts[y * width + x].z
                                   );
                    u = true;
                }
                if ( x - 1 >= 0 )
                {
                    left = glm::vec3(
                                     verts[y * width + x - 1].x - verts[y * width + x].x,
                                     verts[y * width + x - 1].y - verts[y * width + x].y,
                                     verts[y * width + x - 1].z - verts[y * width + x].z
                                     );
                    l = true;
                }
                if ( x + 1 < width )
                {
                    right = glm::vec3(
                                      verts[y * width + x + 1].x - verts[y * width + x].x,
                                      verts[y * width + x + 1].y - verts[y * width + x].y,
                                      verts[y * width + x + 1].z - verts[y * width + x].z
                                      );
                    r = true;
                }
                if ( y + 1 < height && x + 1 < width )
                {
                    down_right = glm::vec3(
                                           verts[(y+1) * width + x + 1].x - verts[y * width + x].x,
                                           verts[(y+1) * width + x + 1].y - verts[y * width + x].y,
                                           verts[(y+1) * width + x + 1].z - verts[y * width + x].z
                                           );
                    dr = true;
                }
                if ( y + 1 < height )
                {
                    down = glm::vec3(
                                     verts[(y+1) * width + x].x - verts[y * width + x].x,
                                     verts[(y+1) * width + x].y - verts[y * width + x].y,
                                     verts[(y+1) * width + x].z - verts[y * width + x].z
                                     );
                    d = true;
                }
                // calculate each available face normal, minimum of two for the 4 corners, 3 for edges, otherwise all 6;
                if (u && ul) norm1 = glm::cross(up, up_left);
                if (u && r) norm2 = glm::cross(right, up);
                if (r && dr) norm3 = glm::cross(down_right, right);
                if (dr && d) norm4 = glm::cross(down, down_right);
                if (d && l) norm5 = glm::cross(left, down);
                if (l && ul) norm6 = glm::cross(up_left, left);
                
                // average these normals
                
                glm::vec3 avg = norm1 + norm2 + norm3 + norm4 + norm5 + norm6;
                avg /= 6;
                // convert to unit length
                avg = glm::normalize(avg);
                // store in normals array
                normals[index] = avg.x;
                normals[index + 1] = avg.y;
                normals[index + 2] = avg.z;
            }
//        for ( int i = 0; i < vertices_size / 12; i ++) outfile << "noisemap: " << nmap->noisemap[i] << "\n";
//        for ( int i = 0; i < vertices_size / 4; i++)
//        {
//            outfile << "vertices, i: " << vertices[i] << ", " << i << "\n";
//            if ( (i+1) % 3 == 0 ) outfile << std::endl;
//        }
    }
    ~Elements()
    {
        delete[] vertices; delete[] indices; outfile.close();
    }
    
    
};


#endif /* Header_h */
