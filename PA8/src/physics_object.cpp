#ifndef PHYSICS_OBJECT
#define PHYSICS_OBJECT
#include "physics_object.h"


// void renderSphere(btRigidBody * sphere)
// {
//     if(sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXY_TYPE)
//     {
//         return;
//     }
//     glColor3f(1.0,0.0,0.0);
//     //cast rigidbody to the sphere shape and then get the radius of the sphere
//     float radiusOfSphere = ((btSphereShape*) sphere->getCollisionShape())->getRadius();
//     btTransform tSphere;
//     sphere->getMotionState()->getWorldTransform(t);

//     //16 element matrix
//     float mat[16];
//     tSphere.getOpenGLMatrix(mat);
//     glPushMatrix();
//     // multiply model_view matrix with mat 
//       glMultMatrixf(mat); // translation, rotation
//     // =============
//     // render sphere here
//     // =============
//     glPopMatrix();
// }

// void renderPlane(btRigidBody * plane)
// {
//     if(plane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXY_TYPE)
//     {
//         return;
//     }

//     //grey color
//     glColor3f(.8,0.8,0.8);
//     //cast rigidbody to the sphere shape and then get the radius of the sphere
//     btTransform tPlane;
//     plane->getMotionState()->getWorldTransform(t);

//     //16 element matrix
//     float mat[16];
//     tPlane.getOpenGLMatrix(mat);
//     glPushMatrix();
//     // multiply model_view matrix with mat 
//       glMultMatrixf(mat); // translation, rotation
//       glBegin(GL_QUADS);
//         glVertex3f(-1000, 0, 1000);
//         glVertex3f(-1000, 0, -1000);
//         glVertex3f(1000, 0, 1000);
//         glVertex3f(1000, 0, -1000);
//       glEnd();
//     glPopMatrix();
// }

// //inside p_world
// for (int i = 0; i<loadedBodies.size(); i++)
// {
//     if(loadedBodies[i]->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXY_TYPE)
//     {
//         renderPlane(loadedBodies[i]);
//     }
//     else if (loadedBodies->getCollisionShape()->getShapeType() == SPHERE_SHAPE_PROXY_TYPE)
//     {
//         renderSphere(loadedBodies[i]);
//     }
// }


// // in engine
// // case_SDL_KEYDOWN:
//     switch(eventi.key.keysym.sym)
//     {
//         // case SDL_ESCAPE:
//         // {
//         //     running = false;
//         //     break;
//         // }

//         // shootSpheres
//         case SDLK_SPACE:
//         {
//             //radius, xyz, mass
//             btRigidBody sphere = addSphere(1.0, cam.getLocation().x, cam.getLocation().y, cam.getLocation().z, 1.0);
//             // add velocity to object (20 = scaler)
//             vector3d look = cam.getVector()*20;
//             //
//             sphere->setLinearVelocity(btVector3(look.x, look.y, look.z));
//             break;
//         }

//     }

#endif