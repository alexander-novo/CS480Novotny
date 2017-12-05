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
        std::vector<int> ballSolids;
        std::vector<int> ballStripes;
        bool sunkSolids[7];
        bool sunkStripes[7];
        // oob = out of bounds
        bool oobStripes[7];
        bool oobSolids[7];
        int cueBall;
        int eightBall;
        bool isNextShotOK = true;
        bool isTurnChange = true;
    };
}

#endif //AMEWORLDCTX_H
