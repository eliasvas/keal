#ifndef PHYS_INC_H
#define PHYS_INC_H
/*
    Features we should support in the future:
    - Oriented Bounded Boxes
    - Joints/IK
    - 3D
*/

/*
    There exist many excellent resources for Game Physics:
    https://github.com/erincatto/box2d-lite (GOAT)
    https://www.amazon.com/Physics-1-Robert-Resnick/dp/0471320579 (Great Textbook)
    https://code.tutsplus.com/series/how-to-create-a-custom-physics-engine--gamedev-12715
    https://github.com/RandyGaul/ImpulseEngine
    https://www.chrishecker.com/Rigid_Body_Dynamics
    https://gafferongames.com/post/fix_your_timestep/
    https://www.amazon.com/Real-Time-Collision-Detection-Interactive-Technology/dp/1558607323
*/
// TODO -- for gameplay purposes we need triggers, meaning entities with colliders that just detect collision and can be customized to for example change screne or do something!
// TODO -- do we need particles to pass through here? maybe a particle can just be a POINT collision PhysicsBody, we need good optimization though
// TODO -- lets ditch O(n^2) and do some spatial partitioning or AABBS better no?
// TODO -- maybe also add layering? (each physics body can have a layer)

#include "collider.h"
#include "physics_body.h"
#include "manifold.h"
#include "world.h"

#endif