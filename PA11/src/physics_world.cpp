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
	dynamicsWorld->setGravity(btVector3(0, -5.6f, 0));
	
	dynamicsWorld->setInternalTickCallback(myTickCallback, static_cast<void*> (this));
	
	//floor
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0, -1, 0));
	btStaticPlaneShape* floor = new btStaticPlaneShape(btVector3(0.0, 1, 0.0), 0);
	btMotionState* motionFloor = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo floorInfo(0, motionFloor, floor);
	floorInfo.m_restitution = 0.1f;
	btRigidBody* floorPlane = new btRigidBody(floorInfo);
	int flags = floorPlane->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT;
	floorPlane->setCollisionFlags(flags);
	floorPlane->setActivationState(DISABLE_DEACTIVATION);
	floorPlane->setFriction(0.15f);
	int everythingElseCollidesWith = COL_BALL | COL_STICK | COL_EVERYTHING_ELSE;
	dynamicsWorld->addRigidBody(floorPlane, COL_EVERYTHING_ELSE, everythingElseCollidesWith);
	
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
	//todo: floor is not a loaded body currently and thus not deleted - causes dynamics world delete to segfault
//	delete dynamicsWorld;
	
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
	bool isDynamic = std::find(objCtx->flags->begin(), objCtx->flags->end(), "dynamic") != objCtx->flags->end();
	if (!isDynamic) {
		mass = 0;
		inertia = {0, 0, 0};
	}
	
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(xPos, yPos, zPos));
	
	if (objCtx->rotationX != 0 || objCtx->rotationY != 0 || objCtx->rotationZ != 0) {
		btQuaternion transformRotationX;
		btQuaternion transformRotationY;
		btQuaternion transformRotationZ;
		// Rotate about axis (using vec3) by an angle.
		transformRotationY.setRotation(btVector3(0.0, 1.0, 0.0), objCtx->rotationY);
		transformRotationX.setRotation(btVector3(1.0, 0.0, 0.0), objCtx->rotationX);
		transformRotationZ.setRotation(btVector3(0.0, 0.0, 1.0), objCtx->rotationZ);
		transform.setRotation(transformRotationX * transformRotationY * transformRotationZ);
	}
	
	
	// Rotate about axis (using vec3) by an angle.
	
	btCollisionShape* newShape;
	
	if (objCtx->shape == 1) {   // SPHERE
		btScalar radius = objCtx->radius;
		newShape = new btSphereShape(radius * objCtx->scale);
	} else if (objCtx->shape ==
	           2) {   // BOX - half-extends are the half the height/width/depth of the box (from a point p out)
		btVector3 boxHalfExtents = {objCtx->widthX * objCtx->scaleX, objCtx->heightY * objCtx->scaleY,
		                            objCtx->lengthZ * objCtx->scaleZ};
		newShape = new btBoxShape(boxHalfExtents);
	} else if (objCtx->shape == 3) {   // CYLINDER
		btVector3 boxHalfExtents = {1 * objCtx->scaleX, 1 * objCtx->scaleY, 1 * objCtx->scaleX};
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
	else if (!isDynamic) {
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
	} else {
		info.m_restitution = 0.7f;
	}
	
	
	
	// takes in the body
	btRigidBody* body = new btRigidBody(info);
	
	body->setUserIndex(loadedBodies.size());
	
	// Set the body to a kinematic object if it is one
	bool isKinematic = std::find(objCtx->flags->begin(), objCtx->flags->end(), "kinematic") != objCtx->flags->end();
	if (isKinematic) {
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
	
	//"Correct" friction
	//body->setFriction(0.125f);
	//body->setRollingFriction(0.05f);
	
	body->setFriction(0.7f);
	body->setRollingFriction(0.1f);
	
	//body->setGravity( btVector3(0,-4, 0));
	// add the object's body to the physics world
	// Balls collide with plunger and invisible wall. Plunger only collides with the ball
	int bodyIndex;
	
	if (objCtx->shape == 1) {
		int ballCollidesWith = COL_STICK | COL_EVERYTHING_ELSE | COL_WALL | COL_BALL;
		dynamicsWorld->addRigidBody(body, COL_BALL, ballCollidesWith);
		loadedBodies.push_back(body);
		bodyIndex = loadedBodies.size() - 1;
	} else {
		bodyIndex = addBody(body);
	}
	
	// Assuming all spheres are balls - add to list of balls in field
	// Used to find which objects to clamp the velocity on
	if (objCtx->shape == 1)   // SPHERE
	{
		ballIndices.push_back(bodyIndex);
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
static void myTickCallback(btDynamicsWorld* world, btScalar timeStep) {
	// This section clamps the velocity (mMaxSpeed) of objects that are set to be clamped
	PhysicsWorld* tempWorld = static_cast<PhysicsWorld*>(world->getWorldUserInfo());
	int mMaxSpeed = 200;
	
	//All of the stuff below doesn't need to be checked if we aren't waiting for the next shot
	if (PhysicsWorld::game->mode != MODE_WAIT_NEXT) return;
	
	bool resting = true;
	if (PhysicsWorld::game != nullptr) {
		int sunkBall = 0;
		float maxSpeed = .02;
		
		// Check GAME STATES to and do appropriate actions
		// i.e. place out of bounds balls back on table
		//		prep cue ball placement on scratch/new game
		//		swap players on turn change
		if (!PhysicsWorld::game->isGameOver && PhysicsWorld::game->mode == MODE_WAIT_NEXT) {
			btVector3 velocity;
			btScalar speed;
			// Check Stripes Resting
			for (int i = 0; i < tempWorld->ballIndices.size(); i++) {
				velocity = (*(tempWorld->getLoadedBodies()))[tempWorld->ballIndices[i]]->getLinearVelocity();
				speed = velocity.length();
				if (speed > maxSpeed) { resting = false;}
				if (PhysicsWorld::game->sunk[i]) { sunkBall++; }
			}
			
			if (resting) {
				if (PhysicsWorld::game->isTurnChange && !PhysicsWorld::game->turnSwapped) {
					PhysicsWorld::game->isPlayer1 = !PhysicsWorld::game->isPlayer1;
					PhysicsWorld::game->turnSwapped = true;
					PhysicsWorld::game->isTurnChange = true;
				} else {
					PhysicsWorld::game->turnSwapped = true;
					PhysicsWorld::game->isTurnChange = true;
				}
				
				if (!PhysicsWorld::game->isNextShotOK && PhysicsWorld::game->turnSwapped) {
					std::cout << "playerShot ready" << std::endl;
					PhysicsWorld::game->isNextShotOK = true;
					
					
					// ToDo: Check if game is win/loss -> activate win/loss
					
					// ToDo::Place out of bounds balls
					// ToDo: Place out of bounds Cue-Ball
					// if cue-ball out of bounds
					// if opposing has balls sunk
					// pull a sunk ball
					// place sunk ball at footspot
					// place cue-ball in kitchen (later: have player set cue-ball
				}
				
				PhysicsWorld::game->mode = MODE_TAKE_SHOT;
			}
		}
	}
	
	btRigidBody* ball;
	btVector3 velocity;
	btScalar speed;
	
	for (int i = 0; i < tempWorld->ballIndices.size(); i++) {
		// Clamp the velocity to help prevent tunneling
		ball = (*(tempWorld->getLoadedBodies()))[tempWorld->ballIndices[i]];
		btVector3 velocity = ball->getLinearVelocity();
		btScalar speed = velocity.length();
		if (speed > mMaxSpeed) {
			velocity *= mMaxSpeed / speed;
			ball->setLinearVelocity(velocity);
		}
		
		// Stripes Ball location trigger
		if (ball->getCenterOfMassPosition().y() < -.1) {
			// if fell through table - is in pocket
			if(tempWorld->ballIndices[i] == PhysicsWorld::game->cueBall) {
				PhysicsWorld::game->mode = MODE_PLACE_CUE;
				PhysicsWorld::game->kMod = (ball->getCenterOfMassPosition().z() > 0)
				                           ? 1 : -1;
				
				PhysicsWorld::game->isPlayer1 = !PhysicsWorld::game->isPlayer1;
				PhysicsWorld::game->turnSwapped = true;
				PhysicsWorld::game->isTurnChange = true;
				
			} else if (!PhysicsWorld::game->sunk[i] && !PhysicsWorld::game->oob[i]) {
				// ToDo: check for more accurate board size (ball must be in bounds)
				if (ball->getCenterOfMassPosition().x() <= 4.5 &&
						ball->getCenterOfMassPosition().x() >= -4.5 &&
						ball->getCenterOfMassPosition().z() < 6 &&
						ball->getCenterOfMassPosition().z() > -6) {
					
					std::cout << "ball sunk: " << i << std::endl;
					
					if(tempWorld->ballIndices[i] == PhysicsWorld::game->eightBall) {
						int numSunk = 0;
						int beginIndex;
						if((PhysicsWorld::game->isPlayer1 && PhysicsWorld::game->isPlayer1Solids) ||
								!(PhysicsWorld::game->isPlayer1 || PhysicsWorld::game->isPlayer1Solids)) {
							beginIndex = 1;
						} else {
							beginIndex = 9;
						}
						
						for(int i = beginIndex; i < beginIndex + 7; i++) {
							if(PhysicsWorld::game->sunk[i]) numSunk++;
						}
						
						std::cout << "Player "
						          << (((numSunk == 7 && PhysicsWorld::game->isPlayer1) || !(numSunk == 7 || PhysicsWorld::game->isPlayer1)) ? 1 : 2)
						          << " wins!" << std::endl;
						
						PhysicsWorld::game->mode = MODE_NONE;
						return;
					}
					
					//Stripes/Solids hasn't been decided yet
					if (!PhysicsWorld::game->isPlayer1Solids && !PhysicsWorld::game->isPlayer1Stripes) {
						if ((PhysicsWorld::game->isPlayer1 && i > 8) || !(PhysicsWorld::game->isPlayer1 || i > 8)) {
							PhysicsWorld::game->isPlayer1Stripes = true;
						} else {
							PhysicsWorld::game->isPlayer1Solids = true;
						}
					}
					
					if ((PhysicsWorld::game->isPlayer1 && PhysicsWorld::game->isPlayer1Solids && i < 8) ||
							!(PhysicsWorld::game->isPlayer1 || PhysicsWorld::game->isPlayer1Solids || i < 8)) {
						PhysicsWorld::game->isTurnChange = false;
						PhysicsWorld::game->turnSwapped = false;
					}
					
					PhysicsWorld::game->sunk[i] = true;
					
					btTransform ballTransform;
					ballTransform.setIdentity();
					ballTransform.setOrigin(btVector3(i, 0, 0));
					
					ball->setWorldTransform(ballTransform);
					ball->setLinearVelocity(btVector3(0, 0, 0));
					ball->setAngularVelocity(btVector3(0, 0, 0));
				} else {
					
					std::cout << "ball oob: " << i << std::endl;
					PhysicsWorld::game->oob[i] = true;
				}
			}
		}
	}
}

#endif

