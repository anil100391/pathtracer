#include "camera.h"
#include "mesh.h"
#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include "renderer.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include "renderer.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using FLOAT = double;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f  = vec3<FLOAT>;
using rayf   = ray<FLOAT>;
using hitf   = hit<FLOAT>;
using scenef = scene<FLOAT>;
using meshf  = mesh<FLOAT>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void buildCornellBoxScene( scenef &world )
{
    material redDiffuse{{1.0f, 0.0f, 0.0f, 1.0f}};
    material blueDiffuse{{1.0f, 0.0f, 1.0f, 1.0f}};
    material greenDiffuse{{1.0f, 1.0f, 0.0f, 1.0f}};
    material whiteEmissive{{1.0f, 1.0f, 1.0f, 1.0f}};
    whiteEmissive.setEmissive( 50.0f );

    auto cornellbox = new meshf( "/home/nebula/code/path_tracer/cornellbox.obj" );
    cornellbox->transform(mat44<FLOAT>::makeRotation(90, 2));
    world
        << new sphere<FLOAT>( {0.4f, 0.0f, -0.6f}, 0.75f * 0.5f, redDiffuse )
        << new sphere<FLOAT>( {-0.4f, 0.5f, -0.6f}, 0.75f * 0.5f, blueDiffuse )
        << new sphere<FLOAT>(
               {-0.4f, -0.5f, -0.6f}, 0.75f * 0.5f, greenDiffuse )
        << new sphere<FLOAT>( {0.0f, 0.0f, 1.25f}, 0.75f * 0.5f, whiteEmissive )
        << cornellbox;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    renderparams rp{256, 256, 4, 64};

    float coord = 2.472f;
    camera<FLOAT> cam{{coord, 0, 0}, {0, 0, 0}, 60};

    scenef world;
    buildCornellBoxScene( world );

    renderer<FLOAT> renderDevice(cam, rp);
    renderDevice.render(world);

    return 0;
}
