//
// Created by Matt on 10/25/17.
//

#ifndef PHYSICS_MODEL_H
#define PHYSICS_MODEL_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "graphics_headers.h"
#include "physics_world.h"
#include "model.h"

class PhysicsModel : public Model
{
    public:
        
        //Load a model from a file
        static PhysicsModel* load(std::string filename, PhysicsWorld *physWorld, bool isDynamic);
   
    private:
        PhysicsModel();
        void loadVertices(aiMesh *mesh, PhysicsModel *newModel);
        void loadIndices(aiMesh *mesh, PhysicsModel *newModel, PhysicsWorld *physWorld, bool isDynamic);
};

#endif /* PHYSICS_MODEL_H */
