//
// Created by matt on 10/27/17.
//

#ifndef GAMEWORLDCTX_H
#define GAMEWORLDCTX_H

#include <vector>
#include "object.h"

class Object;

// A safe space to store the config of our main world
namespace GameWorld
{
    struct ctx
    {
//        Object *surfacePlane;
//        bool surfaceIsDynamic = false;
        std::vector<Object *> worldObjects;
    };
}

#endif //AMEWORLDCTX_H
