#ifndef MATH_H_ 
#define MATH_H_

#include "core_inc.h"


//MATH LIB
typedef union vec2
{
    struct { f32 x,y; };
    struct { f32 u,v; };
    struct { f32 r,g; };
    f32 raw[2];
}vec2;
#define v2(x,y) (vec2){x,y}



typedef union vec3
{
    struct { f32 x,y,z; };
    struct { f32 r,g,b; };
    f32 raw[3];
}vec3;

typedef vec3 color3;
typedef vec3 float3;

typedef union vec4
{
    struct { f32 x,y,z,w; };
    struct { f32 r,g,b,a; };
    f32 raw[4];
}vec4;
typedef vec4 color4;
typedef vec4 float4;

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