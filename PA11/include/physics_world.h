#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H


#include <btBulletDynamicsCommon.h>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include "graphics_headers.h"
#include "gameworldctx.h"

// Collision Types
#define BIT(x) (1<<(x))
enum collisiontypes {
	COL_NOTHING = 0, //<Collide with nothing
	COL_STICK = 1, //<collide with plunger
	COL_BALL = 2, //<Collide with balls
	COL_WALL = 4, //<Collide with walls
	COL_EVERYTHING_ELSE = 8 //<Collide with everything
};

// Callback to determine collisions
static void myTickCallback(btDynamicsWorld *world, btScalar timeStep);

class Object;

class PhysicsWorld {
	public:
		struct Context {
			double rotationX = 0.0f;
			double rotationY = 0.0f;
			double rotationZ = 0.0f;
			float mass = 0;
			float inertia = 0;
			int shape = 0;
			bool isDynamic = false;
			bool isKinematic = false;
			bool isBounceType = false;
			float xLoc = 0;
			float yLoc = 0;
			float zLoc = 0;
			float radius = 0;
			float heightY = 0;
			float widthX = 0;
			float lengthZ = 0;
			float scaleX = 1;
			float scaleY = 1;
			float scaleZ = 1;
			float scale = 1.0f;

			bool hasPhysics = true;
			btRigidBody* physicsBody;
		};
		
		PhysicsWorld();
		
		~PhysicsWorld();
		
		int addBody(btRigidBody* bodyToAdd);
		bool addObject(std::string objectName, btTriangleMesh* objTriMesh);
		int createObject(std::string objectName, btTriangleMesh* objTriMesh, PhysicsWorld::Context* objCtx);
		void update(float dt);
		std::vector<btRigidBody*>* getLoadedBodies();
		
		std::vector<int> ballIndices;
		std::vector<int> singleBallIndex;
		std::vector<int>* currentBallIndices;
		
		static GameWorld::ctx* game;
	
	private:

		// Physics configuration
		btBroadphaseInterface* broadphase;
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;

		// Lists of loaded objects
		std::vector<btRigidBody*> loadedBodies;
	
};



#endif /* PHYSICS_WORLD_H */
