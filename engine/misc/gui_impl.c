#include "engine.h"
const char* gui_vert =
"#version 300 es\n"
"precision mediump float;\n"
"layout(location = 0) in vec2 inPos0;\n"
"layout(location = 1) in vec2 inPos1;\n"
"layout(location = 2) in vec2 inUV0;\n"
"layout(location = 3) in vec2 inUV1;\n"
"layout(location = 4) in vec4 inColor;\n"
"layout(location = 5) in float inCornerRadius;\n"
"layout(location = 6) in float inEdgeSoftness;\n"
"layout(location = 7) in float inBorderThickness;\n"
"\n"
"uniform vec2 winDim;\n"
"\n"
"out vec2 fragUV;\n"
"out vec2 fragDstPos;\n"
"out vec2 fragDstCenter;\n"
"out vec2 fragHalfSize;\n"
"out vec4 fragColor;\n"
"out float fragCornerRadius;\n"
"out float fragEdgeSoftness;\n"
"out float fragBorderThickness;\n"
"\n"
"const vec2 vertices[4] = vec2[4](\n"
"    vec2(-1, -1),\n"
"    vec2(-1, +1),\n"
"    vec2(+1, -1),\n"
"    vec2(+1, +1)\n"
");\n"
"\n"
"void main() {\n"
"    vec2 dstHalfSize = (inPos1 - inPos0) / 2.0;\n"
"    vec2 dstCenter = (inPos1 + inPos0) / 2.0;\n"
"    vec2 dstPos = vertices[gl_VertexID] * dstHalfSize + dstCenter;\n"
"\n"
"    gl_Position = vec4(2.0 * dstPos / winDim - 1.0, 0.0, 1.0);\n"
"    gl_Position.y *= -1.0;\n"
"\n"
"    vec2 uvHalfSize = (inUV1 - inUV0) / 2.0;\n"
"    vec2 uvCenter = (inUV1 + inUV0) / 2.0;\n"
"    vec2 uvPos = vertices[gl_VertexID] * uvHalfSize + uvCenter;\n"
"\n"
"    fragUV = uvPos;\n"
"    fragColor = inColor;\n"
"    fragCornerRadius = inCornerRadius;\n"
"    fragEdgeSoftness = inEdgeSoftness;\n"
"    fragBorderThickness = inBorderThickness;\n"
"    fragDstPos = dstPos;\n"
"    fragDstCenter = dstCenter;\n"
"    fragHalfSize = dstHalfSize;\n"
"}\n";

const char* gui_frag =
"#version 300 es\n"
"precision mediump float;\n"
"in vec2 fragUV;\n"
"in vec2 fragDstPos;\n"
"in vec2 fragDstCenter;\n"
"in vec2 fragHalfSize;\n"
"in vec4 fragColor;\n"
"in float fragCornerRadius;\n"
"in float fragEdgeSoftness;\n"
"in float fragBorderThickness;\n"
"\n"
"out vec4 outColor;\n"
"\n"
"uniform sampler2D texture0;\n"
"\n"
"float roundedRectSDF(vec2 samplePos, vec2 rectCenter, vec2 rectHalfSize, float r) {\n"
"    vec2 d2 = abs(rectCenter - samplePos) - rectHalfSize + vec2(r, r);\n"
"    return min(max(d2.x, d2.y), 0.0) + length(max(d2, 0.0)) - r;\n"
"}\n"
"\n"
"void main() {\n"
"    ivec2 texSize = textureSize(texture0, 0);\n"
"    float col = texture(texture0, fragUV / vec2(texSize)).r;\n"
"    vec4 tex = vec4(col, col, col, col);\n"
"\n"
"    float softness = fragEdgeSoftness + 0.001;\n"
"    float cornerRadius = fragCornerRadius;\n"
"    vec2 softnessPadding = vec2(max(0.0, softness * 2.0 - 1.0), max(0.0, softness * 2.0 - 1.0));\n"
"\n"
"    float dist = roundedRectSDF(fragDstPos, fragDstCenter, fragHalfSize - softnessPadding, cornerRadius);\n"
"    float sdfFactor = 1.0 - smoothstep(0.0, 2.0 * softness, dist);\n"
"\n"
"    float borderFactor = 1.0;\n"
"    if (fragBorderThickness != 0.0) {\n"
"        vec2 interiorHalfSize = fragHalfSize - vec2(fragBorderThickness);\n"
"        float interiorRadiusReduceF = min(interiorHalfSize.x / fragHalfSize.x, interiorHalfSize.y / fragHalfSize.y);\n"
"        float interiorCornerRadius = fragCornerRadius * interiorRadiusReduceF * interiorRadiusReduceF;\n"
"\n"
"        float insideD = roundedRectSDF(fragDstPos, fragDstCenter, interiorHalfSize - softnessPadding, interiorCornerRadius);\n"
"        float insideF = smoothstep(0.0, 2.0 * softness, insideD);\n"
"        borderFactor = insideF;\n"
"    }\n"
"\n"
"    outColor = fragColor * tex * sdfFactor * borderFactor;\n"
"    if (outColor.a < 0.01) discard;\n"
"}\n";


typedef struct guiInstanceData guiInstanceData;

struct guiInstanceData {
    float pos0[2];
    float pos1[2];
    float uv0[2];
    float uv1[2];
    float color[4];
    float corner_radius;
    float edge_softness;
    float border_thickness;
};

oglSP sp = {0};
oglBuf vbo = {0};
oglImage atlas = {0};

void gui_impl_init() {
    guiState *gui_state = gui_state_init();
    gui_set_ui_state(gui_state);

    // create the gui shader
    ogl_sp_init(&sp, gui_vert, gui_frag);
    ogl_sp_add_attrib(&sp, ogl_make_attrib(0,OGL_SHADER_DATA_TYPE_VEC2,sizeof(guiInstanceData),offsetof(guiInstanceData, pos0),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(1,OGL_SHADER_DATA_TYPE_VEC2,sizeof(guiInstanceData),offsetof(guiInstanceData, pos1),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(2,OGL_SHADER_DATA_TYPE_VEC2,sizeof(guiInstanceData),offsetof(guiInstanceData, uv0),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(3,OGL_SHADER_DATA_TYPE_VEC2,sizeof(guiInstanceData),offsetof(guiInstanceData, uv1),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(4,OGL_SHADER_DATA_TYPE_VEC4,sizeof(guiInstanceData),offsetof(guiInstanceData, color),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(5,OGL_SHADER_DATA_TYPE_FLOAT,sizeof(guiInstanceData),offsetof(guiInstanceData, corner_radius),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(6,OGL_SHADER_DATA_TYPE_FLOAT,sizeof(guiInstanceData),offsetof(guiInstanceData, edge_softness),1));
    ogl_sp_add_attrib(&sp, ogl_make_attrib(7,OGL_SHADER_DATA_TYPE_FLOAT,sizeof(guiInstanceData),offsetof(guiInstanceData, border_thickness),1));

    // create the gui InstanceData VBO
    vbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, NULL, 0, sizeof(guiInstanceData));

    // create the gui Atlas texture
    gui_get_ui_state()->atlas.tex.data[0] = 0xFF;
    assert(ogl_image_init(&atlas, &gui_get_ui_state()->atlas.tex.data[0], 1024, 1024, OGL_IMAGE_FORMAT_R8U));
    //assert(ogl_image_init(&atlas, &gui_get_ui_state()->atlas.tex.data[0], 1, 1, OGL_IMAGE_FORMAT_R8U));

}

void gui_impl_update() {
    gui_get_ui_state()->win_dim.x = get_ngs()->win.ww;
    gui_get_ui_state()->win_dim.y = get_ngs()->win.wh;

    //gui_state_update(get_ngs()->dt);
    gui_state_update(1.0/60.0);

	for (u32 mk = 0; mk < 3; mk+=1) {
        guiInputEventNode e = {0};
        e.type = GUI_INPUT_EVENT_TYPE_MOUSE_BUTTON_EVENT;
        e.param0 = mk;
        if (ninput_mkey_pressed(mk)){
            e.param1 = 1;
            gui_input_push_event(e);
        }
        if (ninput_mkey_released(mk)){
            e.param1 = 0;
            gui_input_push_event(e);
        }
    }
    {
        vec2 mp = ninput_get_mouse_pos();
        guiInputEventNode mme = {0};
        mme.type = GUI_INPUT_EVENT_TYPE_MOUSE_MOVE;
        mme.param0 = (s32)mp.x;
        mme.param1 = (s32)mp.y;
        gui_input_push_event(mme);
    }
}

// TODO -- these instance data should be some kind of list in the gui, and COMPOSE a static array for VBO
void gui_impl_render() {
    guiInstanceData *instance_data = (guiInstanceData*)&gui_get_ui_state()->rcmd_buf.commands[0];
    u32 instance_count = gui_render_cmd_buf_count(&gui_get_ui_state()->rcmd_buf);

    //vbo = ogl_buf_make(OGL_BUF_KIND_VERTEX, instance_data, instance_count, sizeof(guiInstanceData));
    ogl_buf_update(&vbo, instance_data, instance_count,sizeof(guiInstanceData));

    ogl_bind_vertex_buffer(&vbo);
    ogl_bind_sp(&sp);

    vec2 windim = v2(gui_get_ui_state()->win_dim.x, gui_get_ui_state()->win_dim.y);
    ogl_sp_set_uniform(&sp, "winDim", OGL_SHADER_DATA_TYPE_VEC2, &windim);
    ogl_bind_image_to_texture_slot(&atlas, 0, 0);
    ogl_draw_instanced(OGL_PRIM_TRIANGLE_STRIP, 0, 4, instance_count);
}