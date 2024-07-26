#ifndef SHADERS_INL
#define SHADERS_INL
// OpenGL ES 3.0 shaders

// TODO -- in the far future, we can use standard GLSL, and depending on 
//backend (desktop/ES3/ES2/?) to transpile, shouldn't be hard 

static const char *batch_vert= 
"#version 300 es\n"
"precision mediump float;\n"
"layout (location = 0) in vec2 in_pos;\n"
"layout (location = 1) in vec2 in_texcoord;\n"
"layout (location = 2) in vec4 in_color;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"out vec2 tc;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"   gl_Position = proj * view * vec4(in_pos.x, in_pos.y, 0.0, 1.0);\n"
"   tc = in_texcoord;\n"
"   color = in_color;\n"
"}\0";

static const char *batch_frag= 
"#version 300 es\n"
"precision mediump float;\n"
"uniform sampler2D Tex;\n"
"in vec2 tc;\n"
"in vec4 color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   vec4 tex_col = texture(Tex, tc/vec2(textureSize(Tex,0)));\n"
"   FragColor = vec4(tex_col.xyz * color.xyz,1.0);\n"
"   FragColor.a = 1.0;\n"
"}\n\0";

#endif