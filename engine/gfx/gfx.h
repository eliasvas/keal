#ifndef GFX_H_
#define GFX_H_
#include "engine.h"
typedef u32 GFX_HANDLE;


typedef enum gfxResType {
    GFX_RESOURCE_TYPE_NIL,
    GFX_RESOURCE_TYPE_BUFFER,
    GFX_RESOURCE_TYPE_IMAGE,
    GFX_RESOURCE_TYPE_SAMPLER,
    GFX_RESOURCE_TYPE_RT,
}gfxResType;

typedef struct gfxResDesc {
    gfxResType type;
    union  {

    }

}gfxResParams;

GFX_HANDLE gfx_res_create(gfxResDesc desc);
b32 gfx_res_destroy(GFX_HANDLE handle);


#endif