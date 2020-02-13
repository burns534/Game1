//
//  VectorUtility.h
//  Game1
//
//  Created by Kyle Burns on 2/9/20.
//  Copyright © 2020 Kyle Burns. All rights reserved.
//

#ifndef VectorUtility_h
#define VectorUtility_h
#pragma once

#include <iostream>
#include <math.h>
/* All vectors default to float precision.
 Use prefix i to specify integer vector.
 Use prefic u to specify unsigned vector.
 Use prefix ul to specify unsigned long vector.
 Use prefit l to specify long vector.
 use prefix d to specify double precision vector.
 use prefit ld to specify long double precision vector.
 */
namespace vu
{
    template <typename T>
    class vec2
    {
    public:
        
        T x,y;
        vec2(){}
        
        vec2(T x, T y)
        {
            this->x = x;
            this->y = y;
        }
        
        vec2(T t)
        {
            x = y = t;
        }
        
        vec2(const vec2 & v)
        {
            this->x = v.x;
            this->y = v.y;
        }
        
        void operator *= (double number)
        {
            x *= number;
            y *= number;
        }
        
        vec2 <T> operator * (double number) const
        {
            return vec2 <T> (this->x * number, this->y * number);
        }
        
        void operator /= (double number)
        {
            x /= number;
            y /= number;
        }
        
        vec2 <T> operator / (double number) const
        {
            return vec2 <T> (this->x / number, this->y / number);
        }
        
        void operator += (double number)
        {
            x += number;
            y += number;
        }
        
        vec2 <T> operator + (double number) const
        {
            return vec2 <T> (this->x + number, this->y + number);
        }
        
        void operator -= (double number)
        {
            x -= number;
            y -= number;
        }
        
        vec2 <T> operator - (double number) const
        {
            return vec2 <T> (this->x - number, this->y - number);
        }
        
        friend std::ostream & operator << (std::ostream &os, const vec2 <T> & v)
        {
            return os << v.x << ", " << v.y;
        }
        
        double length()
        {
            return sqrt(x * x + y * y);
        }
        
        T dot(const vec2 <T> & v)
        {
            return this->x * v.x + this->y * v.y;
        }
        
    };

    template <typename T>
    class vec3
    {
    public:
        T x,y,z;
        
        vec3() {}
        
        vec3(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        
        vec3(T t)
        {
            x = y = z = t;
        }
        
        vec3(const vec3 & v)
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
        }
        
        void operator *= (double number)
        {
           x *= number;
           y *= number;
           z *= number;
        }

        vec3 <T> operator * (double number) const
        {
           return vec3 <T> (this->x * number, this->y * number, this->z * number);
        }

        void operator /= (double number)
        {
           x /= number;
           y /= number;
           z /= number;
        }

        vec3 <T> operator / (double number) const
        {
           return vec3 <T> (this->x / number, this->y / number, this->z / number);
        }

        void operator += (double number)
        {
           x += number;
           y += number;
           z += number;
        }

        vec3 <T> operator + (double number) const
        {
           return vec3 <T> (this->x + number, this->y + number, this->z + number);
        }

        void operator -= (double number)
        {
           x -= number;
           y -= number;
           z -= number;
        }

        vec3 <T> operator - (double number) const
        {
           return vec3 <T> (this->x - number, this->y - number, this->z - number);
        }
        
        vec3 <T> operator - (const vec3 <T> & v) const
        {
            return vec3 <T> (x - v.x, y - v.y, z - v.z);
        }
        
        vec3 <T> operator + (const vec3 <T> & v) const
        {
            return vec3 <T> (x + v.x, y + v.y, z + v.z);
        }

        friend std::ostream & operator << (std::ostream &os, const vec3 <T> & v)
        {
           return os << v.x << ", " << v.y << ", " << v.z;
        }
        
        double length()
        {
            return sqrt(x * x + y * y + z * z);
        }
        
        T dot(const vec3 <T> & v)
        {
            return this->x * v.x + this->y * v.y + this->z * v.z;
        }
        
        vec3 <T> cross(const vec3 <T> & v)
        {
            return vec3 <T> (this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x);
        }
        
        vec3 <double> normalize()
        {
            double mag = this->length();
            return vec3 <double> ((double) this->x / mag, (double) this->y / mag, (double) this-> z / mag);
        }
    };

    template <typename T>
    class vec4
    {
    public:
        T x,y,z,w;

        vec4() {};

        vec4(T x, T y, T z, T w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        vec4(T t)
        {
            x = y = z = w = t;
        }

        vec4(const vec4 & v)
        {
            this->x = v.x;
            this->y = v.y;
            this->x = v.x;
            this->w = v.w;
        }

        void operator *= (double number)
        {
           x *= number;
           y *= number;
           z *= number;
           w *= number;
        }

        vec4 <T> operator * (double number) const
        {
           return vec4 <T> (this->x * number, this->y * number, this->z * number, this-> w * number);
        }

        void operator /= (double number)
        {
            x /= number;
            y /= number;
            z /= number;
            w /= number;
        }

        vec4 <T> operator / (double number) const
        {
           return vec4 <T> (this->x / number, this->y / number, this->z / number, this->w / number);
        }

        void operator += (double number)
        {
            x += number;
            y += number;
            z += number;
            w += number;
        }

        vec4 <T> operator + (double number) const
        {
           return vec4 <T> (this->x + number, this->y + number, this->z + number, this->w + number);
        }

        void operator -= (double number)
        {
            x -= number;
            y -= number;
            z -= number;
            w -= number;
        }

        vec4 <T> operator - (double number) const
        {
           return vec4 <T> (this->x - number, this->y - number, this->z - number, this->w - number);
        }

        friend std::ostream & operator << (std::ostream &os, const vec4 <T> & v)
        {
            return os << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
        }
        
    };

}

#endif /* VectorUtility_h */
