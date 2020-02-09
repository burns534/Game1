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
#include "Droplet.h"
#include "VectorUtility.h"
#define UP 1
#define DOWN 4
#define RIGHT 2
#define LEFT 5
#define UP_LEFT 0
#define DOWN_RIGHT 3

std::ofstream outfile;
struct vec3
{
    float x,y,z;
};

struct ivec3
{
    int x,y,z;
};

class Elements
{
public:
    unsigned* indices;
    float* vertices;
    unsigned vertices_size;
    float * texCoords;
    unsigned texCoords_size;
    NoiseMap* nmap;
    unsigned triangle_number;
    vec3 * verts;
    glm::vec3 * glmverts;
    float * normals;
    unsigned normal_number;
    float * randoms;
    
    glm::vec3 * descent;
    unsigned * directions;
    
    int w,h;
    Elements(int width, int height)
    {
        outfile.open("log.txt");
        // allocate arrays
        w = width; h = height;
        vertices = new float[3 * width * height]; // contains 12 byte objects storing (x,y,z) floats
        verts = new vec3[width * height]; // vertex coordinates
        nmap = new NoiseMap(width, height); // perlin data
        randoms = new float[3 * width * height]; // (x,y,z) for each vertex
        texCoords = new float[2 * width * height]; // (x,y) for each vertex
        descent = new glm::vec3[width * height]; // one normal for each vertex
        directions = new unsigned[width * height];
        glmverts = new glm::vec3[width * height]; // glm::vec3 formatted vertices
        
        texCoords_size = 2 * width * height * 4;
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
                glmverts[y * width + x].x = vertices[index]; // to make normal calculations actually simpler
                // already clamped to [0,1]
                
                vertices[index + 1] = pow(exp(nmap->noisemap[y*width + x]) / 2.72f, 1.5); // perlin z values
                
                if (vertices[index + 1] < 0.44f)
                vertices[index + 1] = 0.4375f + (0.0025f * nmap->n->OctavePerlin( x * 50.34f, y * 50.34f, 0, 8, .50, .003f)); // adds noise to water
                outfile << vertices[index + 1] << "\n";
                verts[y * width + x].y = vertices[index + 1];
                glmverts[y * width + x].y = vertices[index + 1];
                
                //outfile << "(float (2.0f * y) / (height - 1) - 1.0f:" << (float) ( 2.0f * y) / (height - 1) - 1.0f << "\n";
                vertices[index + 2] = static_cast<float> (2.0f * y) / (height - 1) - 1.0f;
                verts[y * width + x].z = vertices[index + 2];
                glmverts[y * width + x].z = vertices[index + 2];
                
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
                
                /* ADD TEXTURE VALUES HERE FOR GRASS TEXTURE */
            }
        for ( unsigned y = 0; y < height ; y += 2)
            for ( unsigned x = 0; x < width ; x += 4)
            {
                /* This will mirror the texture for each tile and allow me to not need two VBO's or more complex techniques.
                 Since it's grass, it shouldn't matter. */
                texCoords[y * width + x] = 0.0f; // top left
                texCoords[y * width + x + 1] = 1.0f;
                
                texCoords[y * width + x + 2] = 1.0f; // top right
                texCoords[y * width + x + 3] = 1.0f;
                
                texCoords[(y+1) * width + x] = 0.0f; // bottom left
                texCoords[(y+1) * width + x + 1] = 0.0f;
                
                texCoords[(y+1) * width + x + 2] = 1.0f; // bottom right
                texCoords[(y+1) * width + x + 3] = 0.0f;
            }
        glm::vec3 right, left, up, down, down_right, up_left, up_right, down_left;
        bool r, l, u, d, dr, ul;
        glm::vec3 norm1, norm2, norm3, norm4, norm5, norm6;
        index = 0;
        normals = new float[3 * width * height];
        glm::vec3 tempFaces[6];
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
                    up_left = glmverts[(y-1) * width + x - 1];
                    outfile << "up_left - glmverts: " << up_left.x - glmverts[y*width+x].x << ", " << up_left.y - glmverts[y*width+x].y << ", " << up_left.z - glmverts[y*width+x].z << "\n";
                    // they appear to be the same.. I'm not really sure what the issue is here.
                    up_left = glm::vec3(
                                        verts[(y-1) * width + x - 1].x - verts[y * width + x].x,
                                        verts[(y-1) * width + x - 1].y - verts[y * width + x].y,
                                        verts[(y-1) * width + x - 1].z - verts[y * width + x].z
                                        );
                    outfile << "up_left: " << up_left.x << ", " << up_left.y << ", " << up_left.z << "\n";
                    ul = true;
                }
                if ( y - 1 >= 0 )
                {
                    up = glmverts[(y-1) * width + x];
//                    up = glm::vec3(
//                                   verts[(y-1) * width + x].x - verts[y * width + x].x,
//                                   verts[(y-1) * width + x].y - verts[y * width + x].y,
//                                   verts[(y-1) * width + x].z - verts[y * width + x].z
//                                   );
                    u = true;
                }
                if ( x - 1 >= 0 )
                {
                    left = glmverts[y * width + x - 1];
//                    left = glm::vec3(
//                                     verts[y * width + x - 1].x - verts[y * width + x].x,
//                                     verts[y * width + x - 1].y - verts[y * width + x].y,
//                                     verts[y * width + x - 1].z - verts[y * width + x].z
//                                     );
                    l = true;
                }
                if ( x + 1 < width )
                {
                    right = glmverts[y * width + x + 1];
//                    right = glm::vec3(
//                                      verts[y * width + x + 1].x - verts[y * width + x].x,
//                                      verts[y * width + x + 1].y - verts[y * width + x].y,
//                                      verts[y * width + x + 1].z - verts[y * width + x].z
//                                      );
                    r = true;
                }
                if ( y + 1 < height && x + 1 < width )
                {
                    down_right = glmverts[(y+1) * width + x + 1];
//                    down_right = glm::vec3(
//                                           verts[(y+1) * width + x + 1].x - verts[y * width + x].x,
//                                           verts[(y+1) * width + x + 1].y - verts[y * width + x].y,
//                                           verts[(y+1) * width + x + 1].z - verts[y * width + x].z
//                                           );
                    dr = true;
                }
                if ( y + 1 < height )
                {
                    down = glmverts[(y+1) * width + x];
//                    down = glm::vec3(
//                                     verts[(y+1) * width + x].x - verts[y * width + x].x,
//                                     verts[(y+1) * width + x].y - verts[y * width + x].y,
//                                     verts[(y+1) * width + x].z - verts[y * width + x].z
//                                     );
                    d = true;
                }
                
                if ( y - 1 >= 0 && x + 1 < width ) up_right = glmverts[(y-1) * width + x + 1]; // up_right vertex
                
                if ( y + 1 < height && x - 1 >= 0 ) down_left = glmverts[(y+1) * width + x - 1]; // down left vertex
                
                // calculate each available face normal, minimum of two for the 4 corners, 3 for edges, otherwise all 6;
                if (u && ul) norm1 = glm::cross((up - glmverts[y * width + x]), up_left);
                if (u && r) norm2 = glm::cross((right - glmverts[y * width + x]), up);
                if (r && dr) norm3 = glm::cross((down_right - glmverts[y * width + x]), right);
                if (dr && d) norm4 = glm::cross((down - glmverts[y * width + x]), down_right);
                if (d && l) norm5 = glm::cross((left - glmverts[y * width + x]), down);
                if (l && ul) norm6 = glm::cross((up_left - glmverts[y * width + x]), left);
                
                // average these normals
//                glm::vec3 avg = norm1 + norm2 + norm3 + norm4 + norm5 + norm6;
//                avg /= 6;
                
                glm::vec3 avg = (norm1 );
                // convert to unit length
                avg = glm::normalize(avg);
                // store in normals array
                normals[index] = avg.x;
                normals[index + 1] = avg.y;
                normals[index + 2] = avg.z;
                
                // store direction vectors to adjacent vertices
                tempFaces[0] = up_left;
                tempFaces[1] = up;
                tempFaces[2] = right;
                tempFaces[3] = down_right;
                tempFaces[4] = down;
                tempFaces[5] = left;
                // will store the direction vector of greatest descent, which will be the velocity vector
                unsigned ind = 0;
                for ( unsigned i = 0; i < 6; i++ )
                {
                    if (tempFaces[i].y < tempFaces[ind].y) ind = i;
                }
                descent[y * w + x] = tempFaces[ind]; // greatest descent for each vertex stored
                directions[y * w + x] = ind;
                //outfile << "Up: " << up.x << ", " << up.y << ", " << up.z << std::endl;
                outfile << "Descent values x,y,z: " << tempFaces[ind].x << ", " << tempFaces[ind].y << ", " << tempFaces[ind].z << "\n";
            }
        
        //Erosion(0.1f, 20.0f, 0.001f, 0.001f);
    }
    ~Elements()
    {
        delete[] vertices; delete[] indices; outfile.close();
    }
    
    void Erosion(float alpha, float beta, float gamma, float threshold)
    {
        double sedimentLoad = 0.1f;
        int currentx, currenty;
        ivec3 direction;
        // for each vertex
        for ( int y = 0; y < h; y += 100 )
            for ( int x = 0; x < w; x += 100 )
            {
                int count = 0;
                currenty = y;
                currentx = x;
                while( verts[currenty * w + currentx].y > 0.44f && count < 20 && currenty > 0 && currenty < h && currentx > 0 && currentx < w)
                    // while we haven't hit water or done more than 20 iterations and won't access bad memory
                {
                    // determine x direction of descent for next array access
                    if (descent[currenty * w + currentx].x > 0) direction.x = 1;
                    else if (descent[currenty * w + currentx].x < 0) direction.x = -1;
                    else direction.x = 0;
                    // determine y direction of descent for next array access
                    if (descent[currenty * w + currentx].y > 0) direction.y = 1;
                    else if (descent[currenty * w + currentx].y < 0) direction.y = -1;
                    else direction.y = 0;
                    
                    // change y value of current vertex by 20%, hopefully enough to notice
                    vertices[3 * (currenty * w + currentx) + 1] = 1.0f + (float) count / - 100.0f;
                    
                    // set next vertex to check
                    currenty += direction.y;
                    currentx += direction.x;
                    count++;
                }
            }
//        glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
//        float acceleration;
//        // will calculate the life of only one raindrop for each vertex on the map
//        double sedimentLoad = 0.01f;
//        float velocity = 0;
//        float drop = 0;
//        int ex, why;
//        // deltaX is 10m for each vertex
//        for ( int y = 0; y < h; y++ )
//            for ( int x = 0; x < w; x++ )
//            {
//                ex = x;
//                why = y;
//                while( verts[why * w + ex].y > 0.44f && sedimentLoad > threshold)
//                {
//                    //outfile << glm::normalize(descent[why * w + ex]).x << ", " << glm::normalize(descent[why * w + ex]).y << ", " << glm::normalize(descent[why * w + ex]).z << "\n";
//                    // calculate acceleration- gravity projected in unit vector direction of greatest descent
//                    float mag = sqrt(pow(descent[why * w + ex].x, 2) + pow(descent[why * w + ex].y, 2) + pow(descent[why * w + ex].z, 2));
//                    acceleration = glm::dot(descent[why * w + ex] / mag, gravity);
//
//                    // calculate magnitude of velocity
//                    velocity = sqrt(velocity * velocity + 20.0f * acceleration) * (1.0f - alpha); // decrease velocity to simulate friction for each iteration
//
//                    drop = (beta - velocity) / beta; // decrease sediment load proportional to difference in velocity and beta
//                    // if ratio is positive then velocity is less than beta and we drop sediment
//                    // if ratio is negative then velocity is greater than beta and we pick up sediment
//                    vertices[3 * (why * w + ex) + y] += gamma * drop; // increase y value by gamma time the sediment drop calculated, deposition inversely proportional to velocity
//                    sedimentLoad -= gamma * drop;
//
//                    if (directions[y * w + x] == UP && why > 0) why--;
//                    else if(directions[y * w + x] == RIGHT && ex < w - 1) ex++;
//                    else if(directions[y * w + x] == DOWN_RIGHT && ex < w - 1 && why < h - 1)
//                    {
//                        ex++; why++;
//                    }
//                    else if(directions[y * w + x] == DOWN && why < h - 1) why++;
//                    else if (directions[y * w + x] == LEFT && ex > 0) ex--;
//                    else if ( ex > 0 && why > 0 )
//                    {
//                        ex--; why--;
//                    }
//                    //ex = direction.x + verts[why * w + ex].x + why * w > 0 ? direction.x + verts[why * w + ex].x : -2; // new x position
//                    //why = direction.z + verts[why * w + ex].z + why * w > 0 ? direction.z + verts[why * w + ex].z : -2; // new z position
//                }
//            }
    }
    
    
};


#endif /* Header_h */
