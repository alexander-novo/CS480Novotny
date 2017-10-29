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
#include "model.h"

class PhysicsModel : public Model
{
    public:
        
        //Load a model from a file
        static PhysicsModel* load(std::string filename, PhysicsWorld *physWorld, PhysicsWorld::Context *worldCtx);
        ~PhysicsModel();
        int getRigidBodyIndex();
   
    private:
        int rigidBodyIndex;
        btTriangleMesh *objTriMesh;
        float mass;
        PhysicsModel();
        void loadVertices(aiMesh *mesh, PhysicsModel *newModel);
        void loadIndices(aiMesh *mesh, PhysicsModel *newModel, PhysicsWorld *physWorld, PhysicsWorld::Context *ctx);
};

#endif /* PHYSICS_MODEL_H */
