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
INLINE f32  vec2_dot(vec2 a, vec2 b)         {return (a.x*b.x)+(a.y*b.y);}
INLINE f32  vec2_len(vec2 a)                 {return sqrtf(vec2_dot(a,a));}
INLINE vec2 vec2_norm(vec2 a)                {f32 vl=vec2_len(a);assert(!equalf(vl,0.0,0.01));return vec2_multf(a,vl);}
INLINE vec2 vec2_rot(vec2 a, f32 angle_rad)  {return v2(a.x*cos(angle_rad)-a.y*sin(angle_rad), a.x*sin(angle_rad)+a.y*cos(angle_rad));}



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
INLINE f32  vec3_dot(vec3 a, vec3 b)         {return (a.x*b.x)+(a.y*b.y)+(a.z*b.z);}
INLINE f32  vec3_len(vec3 a)                 {return sqrtf(vec3_dot(a,a));}
INLINE vec3 vec3_norm(vec3 a)                {f32 vl=vec3_len(a);assert(!equalf(vl,0.0,0.01));return vec3_multf(a,vl);}
INLINE vec3 vec3_cross(vec3 a,vec3 b)        {vec3 res; res.x=(a.y*b.z)-(a.z*b.y); res.y=(a.z*b.x)-(a.x*b.z); res.z=(a.x*b.y)-(a.y*b.x); return (res);} 

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
INLINE f32  vec4_dot(vec4 a, vec4 b)         {return (a.x*b.x)+(a.y*b.y)+(a.z*b.z)+(a.w*b.w);}
INLINE f32  vec4_len(vec4 a)                 {return sqrtf(vec4_dot(a,a));}
INLINE vec4 vec4_norm(vec4 a)                {f32 vl=vec4_len(a);assert(!equalf(vl,0.0,0.01));return vec4_multf(a,vl);}


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

INLINE mat4 m4(void)
{
    mat4 res = {0};
    return res;
}

INLINE mat4 m4d(f32 d)
{
    mat4 res = m4();
    res.col[0][0] = d;
    res.col[1][1] = d;
    res.col[2][2] = d;
    res.col[3][3] = d;
    return res;
}
INLINE mat4 mat4_ortho(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    mat4 res = m4();
    res.col[0][0] = 2.0f / (r - l);
    res.col[1][1] = 2.0f / (t - b);
    res.col[2][2] = 2.0f / (n - f);
    res.col[3][3] = 1.0f;
    res.col[3][0] = (l + r) / (l - r);
    res.col[3][1] = (b + t) / (b - t);
    res.col[3][2] = (f + n) / (n - f);
    return res;
}

INLINE vec3 mat4_extract_pos(mat4 m) {
    return v3(m.col[3][0],m.col[3][1],m.col[3][2]);
}

INLINE mat4 mat4_scale(vec3 s) {
    mat4 res = m4d(1.0f);
    res.col[0][0] = s.x;
    res.col[1][1] = s.y;
    res.col[2][2] = s.z;
    return res;
}
INLINE mat4 mat4_translate(vec3 t) {
    mat4 res = m4d(1.0f);
    res.col[3][0] = t.x;
    res.col[3][1] = t.y;
    res.col[3][2] = t.z;
    return res;
}

INLINE mat4 mat4_mult(mat4 l, mat4 r) {
    mat4 res = m4d(1.0f);
    for (u32 col = 0; col < 4; col+=1)
    {
        for (u32 row = 0; row < 4; row+=1)
        {
            f32 sum = 0;
            for (u32 current_index = 0; current_index < 4; ++current_index)
            {
                sum += (f32)l.col[current_index][row] * (f32)r.col[col][current_index];
            }
            res.col[col][row] = sum;
        }
    }
    return res;
}


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
#define iv2(x,y) (ivec2){x,y}
INLINE s32 ivec2_dot(ivec2 a, ivec2 b)       {return (a.x*b.x)+(a.y*b.y);}
INLINE s32 ivec2_len(ivec2 a)               {return (s32)sqrtf(ivec2_dot(a,a));}

#endif