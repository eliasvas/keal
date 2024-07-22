#ifndef MATH_H
#define MATH_H

#include "base_inc.h"

typedef union vec2
{
    struct { f32 x,y; };
    struct { f32 u,v; };
    struct { f32 r,g; };
    f32 raw[2];
}vec2;
#define v2(x,y) (vec2){x,y}
INLINE vec2 vec2_add(vec2 a, vec2 b)         {return v2(a.x+b.x,a.y+b.y);};
INLINE vec2 vec2_sub(vec2 a, vec2 b)         {return v2(a.x-b.x,a.y-b.y);};
INLINE vec2 vec2_mult(vec2 a, vec2 b)        {return v2(a.x*b.x,a.y*b.y);};
INLINE vec2 vec2_multf(vec2 a, f32 b)        {return v2(a.x*b,a.y*b);};
INLINE vec2 vec2_div(vec2 a, vec2 b)         {return v2(a.x/b.x,a.y/b.y);};
INLINE vec2 vec2_divf(vec2 a, f32 b)         {return v2(a.x/b,a.y/b);};
INLINE vec2 vec2_lerp(vec2 a, vec2 b, f32 x) {return v2(a.x*(1.0-x) + b.x*x,a.y*(1.0-x) + b.y*x);};



typedef union vec3
{
    struct { f32 x,y,z; };
    struct { f32 r,g,b; };
    f32 raw[3];
}vec3;
#define v3(x,y,z) (vec3){x,y,z}
INLINE vec3 vec3_add(vec3 a, vec3 b)         {return v3(a.x+b.x,a.y+b.y,a.z+b.z);};
INLINE vec3 vec3_sub(vec3 a, vec3 b)         {return v3(a.x-b.x,a.y-b.y,a.z-b.z);};
INLINE vec3 vec3_mult(vec3 a, vec3 b)        {return v3(a.x*b.x,a.y*b.y,a.z*b.z);};
INLINE vec3 vec3_multf(vec3 a, f32 b)        {return v3(a.x*b,a.y*b,a.z*b);};
INLINE vec3 vec3_div(vec3 a, vec3 b)         {return v3(a.x/b.x,a.y/b.y,a.z/b.z);};
INLINE vec3 vec3_divf(vec3 a, f32 b)         {return v3(a.x/b,a.y/b,a.z/b);};
INLINE vec3 vec3_lerp(vec3 a, vec3 b, f32 x) {return v3(a.x*(1.0-x) + b.x*x,a.y*(1.0-x) + b.y*x,a.z*(1.0-x)+b.z*x);};


typedef union vec4
{
    struct { f32 x,y,z,w; };
    struct { f32 r,g,b,a; };
    f32 raw[4];
}vec4;
#define v4(x,y,z,w) (vec4){x,y,z,w}
INLINE vec4 vec4_add(vec4 a, vec4 b)         {return v4(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w);};
INLINE vec4 vec4_sub(vec4 a, vec4 b)         {return v4(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w);};
INLINE vec4 vec4_mult(vec4 a, vec4 b)        {return v4(a.x*b.x,a.y*b.y,a.z*b.z,a.w*b.w);};
INLINE vec4 vec4_multf(vec4 a, f32 b)        {return v4(a.x*b,a.y*b,a.z*b,a.w*b);};
INLINE vec4 vec4_div(vec4 a, vec4 b)         {return v4(a.x/b.x,a.y/b.y,a.z/b.z,a.w/b.w);};
INLINE vec4 vec4_divf(vec4 a, f32 b)         {return v4(a.x/b,a.y/b,a.z/b,a.w/b);};
INLINE vec4 vec4_lerp(vec4 a, vec4 b, f32 x) {return v4(a.x*(1.0-x) + b.x*x,a.y*(1.0-x) + b.y*x,a.z*(1.0-x)+b.z*x,a.w*(1.0-x)+b.w*x);};



typedef union mat3
{
    f32 col[3][3];//{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 
    f32 raw[9]; //{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 
}mat3;

typedef union mat4
{
    f32 col[4][4];//{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 
    f32 raw[16]; //{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 
}mat4;


typedef union ivec3
{
    struct { s32 x,y,z; };
    struct { s32 r,g,b; }; 
    s32 raw[3];
}ivec3;

typedef union ivec2
{
    struct { s32 x,y; };
    struct { s32 r,g; };
    s32 raw[2];
}ivec2;

#endif