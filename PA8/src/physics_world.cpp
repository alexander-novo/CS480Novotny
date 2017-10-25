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
    dynamicsWorld->stepSimulation(1/60.0);
} 

PhysicsWorld::PhysicsWorld(float dt)
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

    // The time between ticks of checking for collisions in the world.
    dynamicsWorld->stepSimulation(dt/1000);
} 

PhysicsWorld::~PhysicsWorld()
{
    // Remove Objects/Shapes
    for(int i=0; i<loadedBodies.size(); i++)
    {
        dynamicsWorld->removeCollisionObject(loadedBodies[i]);
        btMotionState * motionState = loadedBodies[i]->getMotionState();
        btCollisionShape * collisionShape = loadedBodies[i]->getCollisionShape();
        delete loadedBodies[i];
        loadedBodies[i] = NULL;
        delete motionState;
        delete collisionShape;
    }

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


bool PhysicsWorld::addBody(btRigidBody* bodyToAdd)
{
    dynamicsWorld->addRigidBody(bodyToAdd);
    loadedBodies.push_back(bodyToAdd);
    return true;
}

bool PhysicsWorld::createObject(std::string objectName, btTriangleMesh *objTriMesh, bool isDynamic)
{

    float xPos, yPos, zPos, mass;
    if(isDynamic)
    {
        mass = 1;
    }
    else
    {
        mass = 0;
    }

    xPos = zPos = yPos = 0;

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(xPos,yPos,zPos));

    // create shape from mesh
    btCollisionShape *newShape = new btBvhTriangleMeshShape(objTriMesh, true);

    // inertia vector
    btVector3 inertia(0,0,0);

    if(mass != 0.0)
        newShape->calculateLocalInertia(mass, inertia);

    // Motion state of the newShape
    btMotionState *motion = new btDefaultMotionState(transform);

    // (mass [0 = static], motionstate, collisionshape, inertia)
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newShape);

    // takes in the body
    btRigidBody * body = new btRigidBody(info);

    // add the object's body to the physics world
    addBody(body);
    loadedPhysicsObjects[objectName] = newShape;
    return true;
}

bool PhysicsWorld::addFloor()
{
    //Set location/orientation of bullet object
    btTransform transform;
    // orientation/position to 0,0,0
    transform.setIdentity();
    transform.setOrigin(btVector3(0,0,0));

    // plane looking up (btVector3), distnace from origin = 0 
    btStaticPlaneShape *plane = new btStaticPlaneShape(btVector3(0.0,1.0,0.0), 0);
    btMotionState *motion = new btDefaultMotionState(transform);

    // (mass [0 = static], motionstate, collisionshape, inertia)
    btRigidBody::btRigidBodyConstructionInfo info(0, motion, plane);

    // takes in the body
    btRigidBody *body = new btRigidBody(info);
    addBody(body);
    return true;
}

bool PhysicsWorld::createDefaultSphere()
{
    float radius, xPos, yPos, zPos, mass;
    mass = radius = 1;
    xPos = zPos = 0;
    yPos = 20; // 20 meters above plane
    btTransform tSphere;
    tSphere.setIdentity();
    tSphere.setOrigin(btVector3(xPos,yPos,zPos));

    // Sphere with radius passed in as parameter
    btSphereShape * sphere = new btSphereShape(radius);

    // inertia vector
    btVector3 inertia(0,0,0);

    if(mass != 0.0)
        sphere->calculateLocalInertia(mass, inertia);

    // Motion state of the sphere
    btMotionState *motion = new btDefaultMotionState(tSphere);

    // (mass [0 = static], motionstate, collisionshape, inertia)
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);

    // takes in the body
    btRigidBody * body = new btRigidBody(info);
    addBody(body);
    return true;
}

std::unordered_map<std::string, btCollisionShape*> PhysicsWorld::getLoadedPhysicsObjects()
{
    return loadedPhysicsObjects;
}

#endif