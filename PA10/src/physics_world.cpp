#ifndef PHYSICS_WORLD
#define PHYSICS_WORLD

#include "physics_world.h"

static void myTickCallback(btDynamicsWorld *world, btScalar timeStep);

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
	dynamicsWorld->setGravity(btVector3(0, -9.81f, -9.81f));

	dynamicsWorld->setInternalTickCallback(myTickCallback, static_cast<void *> (this) );

	addInvisibleWalls();
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
	int everythingElseCollidesWith = COL_BALL | COL_PLUNGER | COL_EVERYTHING_ELSE;
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
	
	if (objCtx->isBounceType) {
		info.m_restitution = 7.0f;
	} else if (objCtx->shape == 1) {
		info.m_restitution = 0.5f;
	} else if (objCtx->isPlunger)	{
		info.m_restitution = 1.0f;
	}
	else	{
		info.m_restitution = 0.7f;
	}


	
	// takes in the body
	btRigidBody* body = new btRigidBody(info);
	
	// Set the body to a kinematic object if it is one
	if (objCtx->isKinematic) {
		flags = body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT;
		body->setCollisionFlags(flags);
	}

	if (objCtx->shape == 1)   // SPHERE (aka a pinball ball)
	{
		// CCD motion based clamping (# meters per sim frame)
		body->setCcdMotionThreshold(.00001);
		// For Spheres use this (< sphere radius):
		body->setCcdSweptSphereRadius(objCtx->radius / 1.6f);
	}
	else if(objCtx->isPlunger)
	{
		body->setCcdMotionThreshold(.00001);
	}

	// add friction to object (.5-.8 for steel)
	// used to reduce the amount of continuous spinning of the ball while on table
	body->setFriction(.5f);
	
	//body->setGravity( btVector3(0,-4, 0));
	// add the object's body to the physics world
	// Balls collide with plunger and invisible wall. Plunger only collides with the ball
	int bodyIndex;

	if(objCtx->shape == 1)
	{
		int ballCollidesWith = COL_PLUNGER | COL_EVERYTHING_ELSE | COL_PLATE | COL_BALL;
		dynamicsWorld->addRigidBody(body, COL_BALL, ballCollidesWith);
		loadedBodies.push_back(body);
		bodyIndex = loadedBodies.size() - 1;
	}
	else if(objCtx->isPlunger)
	{
		int plungerCollidesWith = COL_BALL | COL_EVERYTHING_ELSE;
		dynamicsWorld->addRigidBody(body,COL_PLUNGER, plungerCollidesWith);
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
		ballIndexes.push_back(bodyIndex);
	}
	if (objCtx->isPlunger) // DO this check on the plunger too
	{
		ballIndexes.push_back(bodyIndex);
	}
	
	// Attempting to give an object specific degrees of freedom
	if (objCtx->isPaddle) {
		// Parameters: Body with hinge, point of pivot on object, axis of pivot
		btHingeConstraint* constraint = new btHingeConstraint(*body, btVector3(0, 0, 0), btVector3(0.0, 1.0, 0.0));

		// Sets angle limits
		// If the paddle isn't set to be a right paddle (by rotation of 180 degrees) set as left paddle.
		if(objCtx->rotationY >= M_PI/2 && objCtx->rotationY <= 3*M_PI/2)
		{
			// Adds a motor (like a spring on the hinge) - enabled? velocity scale, impulse scale
			constraint->enableAngularMotor(true, 5, objCtx->mass );
			constraint->setLimit(-M_PI/2.5+objCtx->rotationY, M_PI/4+objCtx->rotationY);
		}
		else
		{
			constraint->enableAngularMotor(true, -5, objCtx->mass);
			constraint->setLimit(-M_PI/4+objCtx->rotationY, M_PI/2.5+objCtx->rotationY);
		}

		
		dynamicsWorld->addConstraint(constraint);
	}

	// Constraints to plunger to keep it in position
	if (objCtx->isPlunger) {
		btTransform plungerTransform;

		body->getMotionState()->getWorldTransform(plungerTransform);
		plungerTransform.operator*(btVector3(0,0,0));
		btSliderConstraint *constraint = new btSliderConstraint(*body, plungerTransform, false);
		body->setLinearFactor(btVector3(0,0,.3));
		body->setAngularFactor(btVector3(0,0,0));
		dynamicsWorld->addConstraint(constraint);
	}
	
	// TODO: add check for if it exists
	loadedPhysicsObjects[objectName] = newShape;
	return bodyIndex;
}

bool PhysicsWorld::addInvisibleWalls() {
	//Set location/orientation of bullet object
	btTransform transform[7];
	// orientation/position to 0,0,0
	for (int i = 0; i < 7; i++) {
		transform[i].setIdentity();
	}

	//floor
	transform[0].setOrigin(btVector3(0, -1, 0));
	//backwall
	transform[1].setOrigin(btVector3(0, 0, -20));
	//frontwall
	transform[2].setOrigin(btVector3(0, 0, 150));
	//leftside
	transform[3].setOrigin(btVector3(-55, 0, 0));
	//rightside
	transform[4].setOrigin(btVector3(50, 0, 0));
	//ceiling
	transform[5].setOrigin(btVector3(0, 6, 0));
	//plunger plate
	transform[6].setOrigin(btVector3(-48, 3, -8.5));
	
	// plane looking up (btVector3), distance from origin = 0
	// btVector3 tells which direction the plane is facing (xyz openGL co-ords)
	btStaticPlaneShape* floor = new btStaticPlaneShape(btVector3(0.0, 1, 0.0), 0);
	btStaticPlaneShape* backWall = new btStaticPlaneShape(btVector3(0.0, 0, 1), 0);
	btStaticPlaneShape* frontWall = new btStaticPlaneShape(btVector3(0.0, 0, -1), 0);
	btStaticPlaneShape* leftSideWall = new btStaticPlaneShape(btVector3(1, 0.0, 0.0), 0);
	btStaticPlaneShape* rightSideWall = new btStaticPlaneShape(btVector3(-1, 0.0, 0.0), 0);
	btStaticPlaneShape* ceiling = new btStaticPlaneShape(btVector3(0.0, -1, 0.0), 0);
	btBoxShape* plungerPlate = new btBoxShape(btVector3(2.5, 3, .0001));
	
	btMotionState* motionFloor = new btDefaultMotionState(transform[0]);
	btMotionState* motionbackWall = new btDefaultMotionState(transform[1]);
	btMotionState* motionfrontWall = new btDefaultMotionState(transform[2]);
	btMotionState* motionleftSideWall = new btDefaultMotionState(transform[3]);
	btMotionState* motionrightSideWall = new btDefaultMotionState(transform[4]);
	btMotionState* motionceiling = new btDefaultMotionState(transform[5]);
	btMotionState* motionplungerplate = new btDefaultMotionState(transform[6]);
	
	// (mass [0 = static], motionstate, collisionshape, inertia)
	btRigidBody::btRigidBodyConstructionInfo floorInfo(0, motionFloor, floor);
	btRigidBody::btRigidBodyConstructionInfo backWallInfo(0, motionbackWall, backWall);
	btRigidBody::btRigidBodyConstructionInfo frontWallInfo(0, motionfrontWall, frontWall);
	btRigidBody::btRigidBodyConstructionInfo leftSideWallInfo(0, motionleftSideWall, leftSideWall);
	btRigidBody::btRigidBodyConstructionInfo rightSideWallInfo(0, motionrightSideWall, rightSideWall);
	btRigidBody::btRigidBodyConstructionInfo ceilingInfo(0, motionceiling, ceiling);
	btRigidBody::btRigidBodyConstructionInfo plungerPlateInfo(0, motionplungerplate, plungerPlate);

	// Gives walls a bit of allowance for bounce
    floorInfo.m_restitution = 0.1f;
	backWallInfo.m_restitution = 0.7f;
	frontWallInfo.m_restitution = 0.7f;
	leftSideWallInfo.m_restitution = 0.7f;
	rightSideWallInfo.m_restitution = 0.7f;
    ceilingInfo.m_restitution = .5f;
	plungerPlateInfo.m_restitution = 0.5f;
	
	
	// takes in the body
	floorPlane = new btRigidBody(floorInfo);
	backWallPlane = new btRigidBody(backWallInfo);
	frontWallPlane = new btRigidBody(frontWallInfo);
	leftSidePlane = new btRigidBody(leftSideWallInfo);
	rightSidePlane = new btRigidBody(rightSideWallInfo);
	ceilingPlane = new btRigidBody(ceilingInfo);
	plungerPlatePlane = new btRigidBody(plungerPlateInfo);
	
	int flags = floorPlane->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT;
	
	floorPlane->setCollisionFlags(flags);
	backWallPlane->setCollisionFlags(flags);
	frontWallPlane->setCollisionFlags(flags);
	leftSidePlane->setCollisionFlags(flags);
	rightSidePlane->setCollisionFlags(flags);
	ceilingPlane->setCollisionFlags(flags);
	plungerPlatePlane->setCollisionFlags(flags);
	
	floorPlane->setActivationState(DISABLE_DEACTIVATION);
	backWallPlane->setActivationState(DISABLE_DEACTIVATION);
	frontWallPlane->setActivationState(DISABLE_DEACTIVATION);
	leftSidePlane->setActivationState(DISABLE_DEACTIVATION);
	rightSidePlane->setActivationState(DISABLE_DEACTIVATION);
	ceilingPlane->setActivationState(DISABLE_DEACTIVATION);
	plungerPlatePlane->setActivationState(DISABLE_DEACTIVATION);

	// add friction to floor (.2 for wood/metal)
	floorPlane->setFriction(0.15f);
	
//	addBody(floorPlane);
	addBody(backWallPlane);
//	addBody(frontWallPlane);
//	addBody(leftSidePlane);
//	addBody(rightSidePlane);
	addBody(ceilingPlane);

	// plate only collides with the ball, not the plunger
	int plateCollidesWith = COL_BALL | COL_EVERYTHING_ELSE;
	dynamicsWorld->addRigidBody(plungerPlatePlane, COL_PLATE, plateCollidesWith);
	loadedBodies.push_back(plungerPlatePlane);

	return true;
}

void PhysicsWorld::renderPlane() {
	if (floorPlane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE) {
		return;
	}
	
	//grey color
	//cast rigidbody to the sphere shape and then get the radius of the sphere
	btTransform tPlane;
	floorPlane->getMotionState()->getWorldTransform(tPlane);
	
	//16 element matrix
	float mat[16];
	tPlane.getOpenGLMatrix(mat);
}

std::vector<btRigidBody*>* PhysicsWorld::getLoadedBodies() {
	return &loadedBodies;
}

void PhysicsWorld::update(float dt) {
	// The time between ticks of checking for collisions in the world.
//    dynamicsWorld->stepSimulation(dt/1000);
//    dynamicsWorld->stepSimulation(1/60.0);
	dynamicsWorld->stepSimulation(dt / 1000, 25);
}

// On each physics tick, clamp the ball velocities
static void myTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	PhysicsWorld *tempWorld = static_cast<PhysicsWorld *>(world->getWorldUserInfo());
	int mMaxSpeed = 200;
	for(int i = 0; i<tempWorld->ballIndexes.size(); i++)
	{
		btVector3 velocity = (*(tempWorld->getLoadedBodies()))[tempWorld->ballIndexes[i]]->getLinearVelocity();
		btScalar speed = velocity.length();
		if(speed > mMaxSpeed)
		{
			velocity *= mMaxSpeed/speed;
			(*(tempWorld->getLoadedBodies()))[tempWorld->ballIndexes[i]]->setLinearVelocity(velocity);
		}
	}
}

#endif

