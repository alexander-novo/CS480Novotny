#ifndef PHYSICS_WORLD
#define PHYSICS_WORLD

#include "physics_world.h"

GameWorld::ctx* PhysicsWorld::game;

PhysicsWorld::PhysicsWorld() {
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
	dynamicsWorld->setGravity(btVector3(0, -1, 0));

	dynamicsWorld->setInternalTickCallback(myTickCallback, static_cast<void *> (this) );
}

PhysicsWorld::~PhysicsWorld() {
	// Remove Objects/Shapes
	for (int i = 0; i < loadedBodies.size(); i++) {
		dynamicsWorld->removeCollisionObject(loadedBodies[i]);
		btMotionState* motionState = loadedBodies[i]->getMotionState();
		btCollisionShape* collisionShape = loadedBodies[i]->getCollisionShape();
		delete loadedBodies[i];
		loadedBodies[i] = nullptr;
		delete motionState;
		delete collisionShape;
	}
	
	// TODO: Destroy models also -> switch to vector instead of map
	
	// Remove World
	delete broadphase;
	delete collisionConfiguration;
	delete dispatcher;
	delete solver;
	delete dynamicsWorld;
	
	broadphase = nullptr;
	collisionConfiguration = nullptr;
	dispatcher = nullptr;
	solver = nullptr;
	dynamicsWorld = nullptr;
}


int PhysicsWorld::addBody(btRigidBody* bodyToAdd) {
	int everythingElseCollidesWith = COL_BALL | COL_STICK | COL_EVERYTHING_ELSE;
	dynamicsWorld->addRigidBody(bodyToAdd, COL_EVERYTHING_ELSE, everythingElseCollidesWith);
	loadedBodies.push_back(bodyToAdd);
	return (int) loadedBodies.size() - 1;
}

int PhysicsWorld::createObject(std::string objectName, btTriangleMesh* objTriMesh, PhysicsWorld::Context* objCtx) {
	
	int flags = 0;
	btVector3 inertia(0, 0, 0);
	float xPos, yPos, zPos, mass;
	mass = objCtx->mass;
	xPos = objCtx->xLoc;
	yPos = objCtx->yLoc;
	zPos = objCtx->zLoc;
	if (objCtx->isDynamic == false) {
		mass = 0;
		inertia = {0, 0, 0};
	}
	
	
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(xPos, yPos, zPos));

	if(objCtx->rotationX != 0 || objCtx->rotationY != 0 || objCtx->rotationZ != 0)
	{
		btQuaternion transformRotationX;
		btQuaternion transformRotationY;
		btQuaternion transformRotationZ;
		// Rotate about axis (using vec3) by an angle.
		transformRotationY.setRotation(btVector3(0.0,1.0,0.0),objCtx->rotationY);
		transformRotationX.setRotation(btVector3(1.0,0.0,0.0),objCtx->rotationX);
		transformRotationZ.setRotation(btVector3(0.0,0.0,1.0),objCtx->rotationZ);
		transform.setRotation(transformRotationX*transformRotationY*transformRotationZ);
	}


	// Rotate about axis (using vec3) by an angle.
	
	btCollisionShape* newShape;

	if (objCtx->shape == 1) {   // SPHERE
		btScalar radius = objCtx->radius;
		newShape = new btSphereShape(radius * objCtx->scale);
	} else if (objCtx->shape ==
	           2) {   // BOX - half-extends are the half the height/width/depth of the box (from a point p out)
		btVector3 boxHalfExtents = {objCtx->widthX*objCtx->scaleX, objCtx->heightY*objCtx->scaleY, objCtx->lengthZ*objCtx->scaleZ};
		newShape = new btBoxShape(boxHalfExtents);
	} else if (objCtx->shape == 3) {   // CYLINDER
		btVector3 boxHalfExtents = {1*objCtx->scaleX, 1*objCtx->scaleY, 1*objCtx->scaleX};
		newShape = new btCylinderShape(boxHalfExtents);
	} else if (objCtx->shape == 4) {   // PLANE(ground)
		// The plane's normal (direction, [0,1,0] means it faces up in the y direction
		// defined by a 1 in the config file object's width(x), height(y), or depth(z)
		btVector3 planeNormal = {objCtx->widthX, objCtx->heightY, objCtx->lengthZ};
		// using default of 1 for thickness of plane
		btScalar planeConstant = 0;
		newShape = new btStaticPlaneShape(planeNormal, planeConstant);
	}
		// create shape from mesh
	else if (!(objCtx->isDynamic)) {
		newShape = new btBvhTriangleMeshShape(objTriMesh, true);
	} else {
		newShape = new btConvexTriangleMeshShape(objTriMesh, true);
	}
	
	// inertia vector
	
	
	if (mass != 0.0)
		newShape->calculateLocalInertia(mass, inertia);
	
	// Motion state of the newShape
	btMotionState* motion = new btDefaultMotionState(transform);
	
	// (mass [0 = static], motionstate, collisionshape, inertia)
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newShape, inertia);
	
	if (objCtx->shape == 1) {
		info.m_restitution = 0.75f;
	}
	else	{
		info.m_restitution = 0.7f;
	}


	
	// takes in the body
	btRigidBody* body = new btRigidBody(info);
	
	body->setUserIndex(loadedBodies.size());

	// Set the body to a kinematic object if it is one
	if (objCtx->isKinematic) {
		flags = body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT;
		body->setActivationState(DISABLE_DEACTIVATION);
		body->setCollisionFlags(flags);
	}

	if (objCtx->shape == 1)   // SPHERE (aka a pinball ball)
	{
		// CCD motion based clamping (# meters per sim frame)
		body->setCcdMotionThreshold(.00001);
		// For Spheres use this (< sphere radius):
		body->setCcdSweptSphereRadius(objCtx->radius / 1.6f);
		body->setActivationState(DISABLE_DEACTIVATION);
	}

	// add friction to object (.5-.8 for steel)
	// used to reduce the amount of continuous spinning of the ball while on table
	body->setFriction(0.7f);
	body->setRollingFriction(0.05f);
	
	//body->setGravity( btVector3(0,-4, 0));
	// add the object's body to the physics world
	// Balls collide with plunger and invisible wall. Plunger only collides with the ball
	int bodyIndex;

	if(objCtx->shape == 1)
	{
		int ballCollidesWith = COL_STICK | COL_EVERYTHING_ELSE | COL_WALL | COL_BALL;
		dynamicsWorld->addRigidBody(body, COL_BALL, ballCollidesWith);
		loadedBodies.push_back(body);
		bodyIndex = loadedBodies.size() - 1;
	}
	else
	{
		bodyIndex = addBody(body);
	}

	// Assuming all spheres are balls - add to list of balls in field
	// Used to find which objects to clamp the velocity on
	if (objCtx->shape == 1)   // SPHERE
	{
		ballIndices.push_back(bodyIndex);

		// Keep first Ball as Single
		if(singleBallIndex.size() == 0)
		{
			singleBallIndex.push_back(bodyIndex);
		}
	}

	// TODO: add check for if it exists
	return bodyIndex;
}

std::vector<btRigidBody*>* PhysicsWorld::getLoadedBodies() {
	return &loadedBodies;
}

void PhysicsWorld::update(float dt) {
	// The time between ticks of checking for collisions in the world.
	dynamicsWorld->stepSimulation(dt / 1000, 25);
}

// On each physics tick, clamp the ball velocities
static void myTickCallback(btDynamicsWorld *world, btScalar timeStep)
{

	// This section clamps the velocity (mMaxSpeed) of objects that are set to be clamped
	PhysicsWorld *tempWorld = static_cast<PhysicsWorld *>(world->getWorldUserInfo());
	int mMaxSpeed = 200;
	static long long score = 0;
	static bool resting = true;

	if(PhysicsWorld::game != nullptr)
	{
		int sunkBall = 0;
//		bool sunkCueBall;
//		bool sunk8Ball;
		float maxSpeed = .02;
		if(!PhysicsWorld::game->isGameOver)
		{
			btVector3 velocity;
			btScalar speed;
			// Check Stripes Resting
			for(int i = 0; i< PhysicsWorld::game->ballStripes.size(); i++)
			{
				velocity = (*(tempWorld->getLoadedBodies()))[(PhysicsWorld::game->ballStripes[i])]->getLinearVelocity();
				speed = velocity.length();
				if(speed > maxSpeed) {resting = false;}
				if(PhysicsWorld::game->sunkStripes[i]){sunkBall++;}
			}
			// Check Solids Resting
			for(int i = 0; i< PhysicsWorld::game->ballSolids.size(); i++)
            {
				velocity = (*(tempWorld->getLoadedBodies()))[(PhysicsWorld::game->ballSolids[i])]->getLinearVelocity();
				speed = velocity.length();
				if(speed > maxSpeed) {resting = false;}
				if(PhysicsWorld::game->sunkSolids[i]){sunkBall++;}
			}
			// Check cue-ball and 8-ball resting
			velocity = (*(tempWorld->getLoadedBodies()))[(PhysicsWorld::game->cueBall)]->getLinearVelocity();
			speed = velocity.length();
			if(speed > maxSpeed) {resting = false;}
			velocity = (*(tempWorld->getLoadedBodies()))[(PhysicsWorld::game->eightBall)]->getLinearVelocity();
			speed = velocity.length();
			if(speed > maxSpeed) {resting = false;}

			if(resting)
			{
				if(!PhysicsWorld::game->isNextShotOK)
				{
					// ToDo: Check if game is win/loss -> activate win/loss
					if(PhysicsWorld::game->isTurnChange)
					{
						// ToDo: Change turns (in menu)
					}
					// ToDo::Place out of bounds balls
					// ToDo: Place out of bounds Cue-Ball
						// if cue-ball out of bounds
							// if opposing has balls sunk
								// pull a sunk ball
								// place sunk ball at footspot
							// place cue-ball in kitchen (later: have player set cue-ball
				}
			}

		}
	}

	// Traverse all the striped balls and check if they are out of bounds on speed or location
	for(int i = 0; i< PhysicsWorld::game->ballStripes.size(); i++)
	{

//        std::cout << "ball indices: " << i << std::endl;
        // Clamp the velocity to help prevent tunneling
		btVector3 velocity = (*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->getLinearVelocity();
		btScalar speed = velocity.length();
		if(speed > mMaxSpeed)
		{
			velocity *= mMaxSpeed/speed;
			(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->setLinearVelocity(velocity);
		}

        // Ball location trigger
		if((*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->getCenterOfMassPosition().y() < -.1)
		{
			// if fell through table - is in pocket
			if(PhysicsWorld::game->sunkStripes[i] != true && PhysicsWorld::game->oobStripes[i] != true)
			{
				if
				(
					// ToDo: check for more accurate board size (ball must be in bounds)
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->getCenterOfMassPosition().x() <= 4.5 &&
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->getCenterOfMassPosition().x() >= -4.5 &&
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->getCenterOfMassPosition().z() < 6 &&
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->getCenterOfMassPosition().z() > -6
				)
				{

					std::cout << "ball sunk: " << i+9 << std::endl;
					btTransform ballTransform;
					ballTransform.setIdentity();
					ballTransform.setOrigin(btVector3(i, 0, 0));
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->setWorldTransform(ballTransform);
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->setLinearVelocity(btVector3(0, 0, 0));
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballStripes[i]]->setAngularVelocity(btVector3(0, 0, 0));
					if(PhysicsWorld::game->sunkStripes[i] != true)
					{
						PhysicsWorld::game->sunkStripes[i] = true;
					}
				}
				else
				{

					std::cout << "ball oob: " << i+9 << std::endl;
					PhysicsWorld::game->oobStripes[i] = true;
				}
			}
		}

		// Ball location trigger
		if((*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->getCenterOfMassPosition().y() < -.1)
		{
			// if fell through table - is in pocket
			if(PhysicsWorld::game->sunkStripes[i] != true && PhysicsWorld::game->oobStripes[i] != true)
			{
				if
						(
					// ToDo: check for more accurate board size (ball must be in bounds)
						(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->getCenterOfMassPosition().x() <= 4.5 &&
						(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->getCenterOfMassPosition().x() >= -4.5 &&
						(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->getCenterOfMassPosition().z() < 6 &&
						(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->getCenterOfMassPosition().z() > -6
						)
				{

					std::cout << "ball sunk: " << i+9 << std::endl;
					btTransform ballTransform;
					ballTransform.setIdentity();
					ballTransform.setOrigin(btVector3(i, 0, 0));
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->setWorldTransform(ballTransform);
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->setLinearVelocity(btVector3(0, 0, 0));
					(*(tempWorld->getLoadedBodies()))[PhysicsWorld::game->ballSolids[i]]->setAngularVelocity(btVector3(0, 0, 0));
					if(PhysicsWorld::game->sunkSolids[i] != true)
					{
						PhysicsWorld::game->sunkSolids[i] = true;
					}
				}
				else
				{

					std::cout << "ball oob: " << i+9 << std::endl;
					PhysicsWorld::game->oobSolids[i] = true;
				}
			}
		}
	}


	// This section is to detect collisions
	// (for bumper interactions/scoring)
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold =  world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();
		
		if(obA->getUserIndex() == -1 || obB->getUserIndex() == -1) continue;


		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.1f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;
				
				Object* obj1 = nullptr;
				Object* obj2 = nullptr;

				for(auto& obj : PhysicsWorld::game->worldObjects ) {
					if(obj->ctx.physicsBody->getUserIndex() == obA->getUserIndex()) {
						obj1 = obj;
					}
					if(obj->ctx.physicsBody->getUserIndex() == obB->getUserIndex()) {
						obj2 = obj;
					}
				}

//				if(obj1->ctx.shape == 1 && obj2->ctx.bumperLight != nullptr) { // Cylinder Bumpers
//					score += 125;
//					*obj2->ctx.bumperLight = 500;
//					obj2->ctx.expansionTimer = 250;
//					obj2->ctx.tempScale = obj2->ctx.scale * 1.1f;
//
//				}
//				else if((obj1->ctx.shape == 1) && (obj2->ctx.isBounceType == true) && (pt.getDistance() < 0.0f)) // Other Bumpers
//				{
//					score+= 50;
//				}

			}
		}
	}
}

#endif

