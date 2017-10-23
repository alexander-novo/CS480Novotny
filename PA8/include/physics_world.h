#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H


#include <btBulletDynamicsCommon.h>
#include <map>
#include <unordered_map>
#include <string>


class PhysicsWorld
{
    public:
        PhysicsWorld();
        ~PhysicsWorld();
        bool addObject(std::string objectName, btTriangleMesh *objTriMesh);
        std::unordered_map<std::string, btCollisionShape*> getLoadedPhysicsObjects();

    private:
        btBroadphaseInterface *broadphase;
        btDefaultCollisionConfiguration *collisionConfiguration;
        btCollisionDispatcher * dispatcher;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld * dynamicsWorld;
        std::unordered_map<std::string, btCollisionShape*> loadedPhysicsObjects;

};
#endif
