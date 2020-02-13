//
//  Header.h
//  Game1
//
//  Created by Kyle Burns on 1/22/20.
//  Copyright © 2020 Kyle Burns. All rights reserved.
//

#ifndef Header_h
#define Header_h
#pragma once

#include "NoiseMap.h"
#include <fstream>
#include "VectorUtility.h"

std::ofstream outfile;

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
    vu::vec3 <double> * verts;
    vu::vec3 <double> * glmverts;
    float * normals;
    unsigned normal_number;
    float * randoms;
    
    vu::vec3 <double> * descent;
    
    int w,h;
    Elements(int width, int height)
    {
        outfile.open("log.txt");
        // allocate arrays
        w = width; h = height;
        vertices = new float[3 * width * height]; // contains 12 byte objects storing (x,y,z) floats
        verts = new vu::vec3 <double> [width * height]; // vertex coordinates
        nmap = new NoiseMap(width, height); // perlin data
        randoms = new float[3 * width * height]; // (x,y,z) for each vertex
        texCoords = new float[2 * width * height]; // (x,y) for each vertex
        descent = new vu::vec3 <double> [width * height]; // one normal for each vertex
       
        texCoords_size = 2 * width * height * 4;
        triangle_number = 2 * ( width - 1 ) * ( height - 1 );
        vertices_size = 3 * width * height * 4; // floats are 4 bytes
        //double correctionFactor = 0.0025f;
        // fill vertices with appropriate data
        unsigned index = 0;
        for ( int y = 0; y < height; y++)
            for ( int x = 0; x < width; x++, index += 3)
            {
                // need to put in device coords
                vertices[index] = (2.0f * x / (width - 1)) - 1.0f;
                verts[y * width + x].x = vertices[index]; // to make normal calculations simpler
                // already clamped to [0,1]
                
                // perlin z values smoothed by exponential function and then raised to 1.5
                /* This was done to address the steep slope of the beaches/sand shoals on the map. */
                vertices[index + 1] = exp(2.8 * nmap->noisemap[y*width + x]) / 20.0f; // 403 ~= e ^ 6
                
                if (vertices[index + 1] < 0.165f)
                    vertices[index + 1] = 0.1625f;// + (correctionFactor * nmap->n->OctavePerlin( x * 50.34f, y * 50.34f, 0, 8, .50, .003f)); // adds noise to water
                verts[y * width + x].y = vertices[index + 1];
                
                //outfile << "(float (2.0f * y) / (height - 1) - 1.0f:" << (float) ( 2.0f * y) / (height - 1) - 1.0f << "\n";
                vertices[index + 2] = (2.0f * y) / (height - 1) - 1.0f;
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
        
        vu::vec3 <double> right, left, up, down, down_right, up_left, up_right, down_left;
        bool r, l, u, d, dr, ul;
        vu::vec3 <double> norm1, norm2, norm3, norm4, norm5, norm6;
        index = 0;
        normals = new float[3 * width * height];
        vu::vec3 <double> tempFaces[8];
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
                    up_left = verts[(y-1) * width + x - 1] - verts[y * width + x];
                    ul = true;
                }
                if ( y - 1 >= 0 )
                {
                    up = verts[(y-1) * width + x] - verts[y * width + x];
                    u = true;
                }
                if ( x - 1 >= 0 )
                {
                    left = verts[y * width + x - 1] - verts[y * width + x];
                    l = true;
                }
                if ( x + 1 < width )
                {
                    right = verts[y * width + x + 1] - verts[y * width + x];
                    r = true;
                }
                if ( y + 1 < height && x + 1 < width )
                {
                    down_right = verts[(y+1) * width + x + 1] - verts[y * width + x];
                    dr = true;
                }
                if ( y + 1 < height )
                {
                    down = verts[(y+1) * width + x] - verts[y * width + x];
                    d = true;
                }
                
                if ( y - 1 >= 0 && x + 1 < width ) up_right = verts[(y-1) * width + x + 1] - verts[y * width + x]; // up_right vertex
                
                if ( y + 1 < height && x - 1 >= 0 ) down_left = verts[(y+1) * width + x - 1] - verts[y * width + x]; // down left vertex
                
                // calculate each available face normal, minimum of two for the 4 corners, 3 for edges, otherwise all 6;
                if (u && ul) norm1 = up_left.cross(up) * -1;
                if (u && r) norm2 = up.cross(right) * -1;
                if (r && dr) norm3 = right.cross(down_right) * -1;
                if (dr && d) norm4 = down_right.cross(down) * -1;
                if (d && l) norm5 = down.cross(left) * -1;
                if (l && ul) norm6 = left.cross(up_left) * -1;
                
//                if(u && ul) norm1 = up_left.cross(up - verts[y * width + x]) * -1;
//                if(u && r) norm2 = up.cross(right - verts[y * width + x]) * -1;
//                if(r && dr) norm3 = right.cross(down_right - verts[y * width + x]) * -1;
//                if(dr && d) norm4 = down_right.cross(down - verts[y * width + x]) * -1;
//                if(d && l) norm5 = down.cross(left - verts[y * width + x]) * -1;
//                if(l && ul) norm6 = left.cross(up_left - verts[y * width + x]) * -1;
                
                
              //   average these normals
                vu::vec3 <double> avg = norm1 + norm2 + norm3 + norm4 + norm5 + norm6;
                avg /= 6;
                
                //vu::vec3 <double> avg = (norm1); // this is why only the up vector affected the shading earlier
                // convert to unit length
                avg = avg.normalize();
                // store in normals array
                normals[index] = avg.x;
                normals[index + 1] = avg.y;
                normals[index + 2] = avg.z;
                //outfile << "phong normal: " << avg << std::endl;
                // store direction vectors to adjacent vertices
                tempFaces[0] = up_left;
                tempFaces[1] = up;
                tempFaces[2] = right;
                tempFaces[3] = down_right;
                tempFaces[4] = down;
                tempFaces[5] = left;
                tempFaces[6] = up_right;
                tempFaces[7] = down_left;
                // will store the direction vector of greatest descent, which will be the velocity vector
                
                unsigned ind = 0;
                bool flag = true;
                for ( unsigned i = 0; i < 8; i++ )
                {
                    // if direction vector is less than current most downward
                    if (tempFaces[i].y  < tempFaces[ind].y && tempFaces[i].y < 0.0f )
                    {
                        ind = i;
                        flag = false;
                    }
                }
                // store maximum descent vector
                descent[y * w + x] = tempFaces[ind]; // greatest descent for each vertex stored
                
                if (flag) descent[y * w + x].y = -37.0f;
                //outfile << "Up: " << up.x << ", " << up.y << ", " << up.z << std::endl;
//                outfile << "Descent values x,y,z: " << tempFaces[ind].x << ", " << tempFaces[ind].y << ", " << tempFaces[ind].z << "\n";
            }
        
        //for ( int i = 0; i < 1; i++) Erosion(0.1f, 10.0f, 0.0001f, 0.05f);
    }
    ~Elements()
    {
        delete[] vertices; delete[] indices; delete[] verts; delete[] randoms; outfile.close();
    }
    
    void Erosion(float alpha, float beta, float gamma, float threshold)
    {
        int currentx, currenty;
        vu::vec2 <int> direction;
        vu::vec3 <double> gravity(0.0f, -9.8f, 0.0f);
        float acceleration = 0.0f;
        float velocity = 0.0f;
        float drop = 0.0f;
        double sedimentLoad;
        // for each vertex
        for ( int y = 0; y < h; y ++ )
            for ( int x = 0; x < w; x ++ )
            {
                int count = 0;
                currenty = y;
                currentx = x;
                sedimentLoad = 0.0f;
                while(verts[currenty * w + currentx].y > 0.44f && currenty > 0 && currenty < h && currentx > 0 && currentx < w)
                    // while we haven't hit water or done more than 20 iterations and won't access bad memory
                {
                    // determine if there will be a false alarm
//                    if (descent[currenty * w + currentx].y == -37.0f)
//                    {
//                        // there is no good descent;
//                        break;
//                    }
                    
                    // determine x direction of descent for next array access
                    if (descent[currenty * w + currentx].x > 0) direction.x = 1;
                    else if (descent[currenty * w + currentx].x < 0) direction.x = -1;
                    else direction.x = 0;
                    //outfile << "here: " << currentx << ", " << currenty << ", " << descent[currenty * w + currentx] << std::endl;
                    // determine z direction of descent for next array access
                    if (descent[currenty * w + currentx].z > 0) direction.y = 1;
                    else if (descent[currenty * w + currentx].z < 0) direction.y = -1;
                    else direction.y = 0;
                    
                    // calculate acceleration
                    acceleration = gravity.dot(descent[currenty * w + currentx].normalize());
                    
                    
                    //outfile << "acceleration: " << acceleration << "descent: " << descent << "\n";
                    
                    
                    velocity = sqrt(velocity * velocity + 20.0f * acceleration) * (1.0f - alpha); // decrease velocity to simulate friction for each iteration
                    outfile << "velocity: " << velocity << "\n";
                    
                    // water droplet is no longer moving, drop sediment and terminate
                    if (velocity < threshold)
                    {
                        verts[currenty * w + currentx].y += sedimentLoad;
                        break;
                    }
                    
                    drop = (beta - velocity) / beta; // decrease sediment load proportional to difference in velocity and beta
                    // if ratio is positive then velocity is less than beta and we drop sediment
                    // if ratio is negative then velocity is greater than beta and we pick up sediment
                    
                    vertices[3 * (currenty * w + currentx) + 1] += gamma * drop;
                    verts[currenty * w + currentx].y += gamma * drop; // increase y value by gamma time the sediment drop calculated, deposition inversely proportional to velocity
                    if ( gamma * drop > 0 )
                    sedimentLoad -= gamma * drop;
                    //outfile << "gamma: " << gamma * drop << std::endl;
                    // change y value of current vertex by 20%, hopefully enough to notice
                    //vertices[3 * (currenty * w + currentx) + 1];
                    
                    here:
                    // set next vertex to check
                    currenty += direction.y;
                    currentx += direction.x;
                    count++;
                }
                outfile << "Count: " << count << "\n";
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
