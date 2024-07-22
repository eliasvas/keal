#ifndef GLB_H
#define GLB_H

typedef struct InstanceData {
    float pos0[2];
    float pos1[2];
    float uv0[2];
    float uv1[2];
    float color[4];
    float corner_radius;
    float edge_softness;
    float border_thickness;
}InstanceData;


void platform_deinit(void);
void platform_update(void);
void platform_render(InstanceData *data, u32 instance_count);
void platform_init(void);

#endif