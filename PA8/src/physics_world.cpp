#ifndef PHYSICS_WORLD
#define PHYSICS_WORLD

#include "physics_world.h"

PhysicsWorld::PhysicsWorld()
{
    // ====================== <Initialization> ===================

    // Create a *broadphase*
    // Used to check for collisions between objects. (Also helps eliminate object pairs that shouldn't collide)
    broadphase = new btDbvtBroadphase();

    // Create a *collision configuration*
    // The collision algorithm that can be used to regiser a callback that filters overlapping broadphase
    // proxies so that the collisions are not processed by the rest of the system.
    // ....whatever that means
    collisionConfiguration = new btDefaultCollisionConfiguration();

    // Create a *dispatcher*
    // Takes collision config pointer as a parameter
    // Along with collisionConfig, it sends events to the objects 
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    //Create a solver
    // Causes objects to interact properly, taking into account gravity, forces, collisions, hinge constraints
    // World and Objects are both handled with this pointer.
    solver = new btSequentialImpulseConstraintSolver;

    // Create World!
    // World uses the initialization parameters
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // ====================== </Initialization> ==================

    // Earth Gravity in the Y direction
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0)); 
} 

PhysicsWorld::~PhysicsWorld()
{

    /*
        // remove objects
        // Pointer clean-up 
        // Shapes
        delete flatPlaneShape;
        delete sphereShape;
        delete boxShape;
        delete objTriMesh;

        flatPlaneShape = NULL;
        sphereShape = NULL;
        boxShape = NULL;
        objTriMesh = NULL;
    */

    // Remove World
    delete broadphase;
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;
    delete dynamicsWorld;

    broadphase = NULL;
    collisionConfiguration = NULL;
    dispatcher = NULL;
    solver = NULL;
    dynamicsWorld = NULL;
}

bool PhysicsWorld::addObject(std::string objectName, btTriangleMesh *objTriMesh)
{
    btCollisionShape *newShape = new btBvhTriangleMeshShape(objTriMesh, true);
    loadedPhysicsObjects[objectName] = newShape;
    return true;
}

std::unordered_map<std::string, btCollisionShape*> PhysicsWorld::getLoadedPhysicsObjects()
{
    return loadedPhysicsObjects;
}

#endif