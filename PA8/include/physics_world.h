#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H


#include <btBulletDynamicsCommon.h>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>


class PhysicsWorld
{
    public:
        PhysicsWorld();
        ~PhysicsWorld();
        PhysicsWorld(float dt);
        bool addBody(btRigidBody *bodyToAdd);
        bool addObject(std::string objectName, btTriangleMesh *objTriMesh);
        std::unordered_map<std::string, btCollisionShape*> getLoadedPhysicsObjects();
        bool createObject(std::string objectName, btTriangleMesh *objTriMesh, bool isDynamic);
        bool createDefaultSphere();
        bool createFloor();
       

    private:
        bool addFloor();

        btBroadphaseInterface *broadphase;
        btDefaultCollisionConfiguration *collisionConfiguration;
        btCollisionDispatcher * dispatcher;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld * dynamicsWorld;
        std::unordered_map<std::string, btCollisionShape*> loadedPhysicsObjects;
        std::vector<btRigidBody*> loadedBodies;

};

#endif /* PHYSICS_WORLD_H */
