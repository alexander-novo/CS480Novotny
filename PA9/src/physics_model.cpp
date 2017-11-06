// physics_model
// 10-25-17

#ifndef PHYSICS_MODEL
#define PHYSICS_MODEL

#include "physics_model.h"

PhysicsModel::PhysicsModel() : rigidBodyIndex(0) {}

PhysicsModel::~PhysicsModel() {
	// clean up Bullet's meshes (ha ha ha)
	delete objTriMesh;
	objTriMesh = nullptr;
}

PhysicsModel* PhysicsModel::load(std::string filename, PhysicsWorld* physWorld, PhysicsWorld::Context* worldCtx) {
	static std::unordered_map<std::string, PhysicsModel*> loadedModels;
	// If we already loaded this model before
	// add another physics object, but use the same model
//    if(loadedModels.find(filename) != loadedModels.end()) {
//        // TODO: Add already loaded model as new object in the physics world
//        return loadedModels[filename];
//    }
	
	PhysicsModel* newModel = new PhysicsModel();
	newModel->mass = worldCtx->mass;
	
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast);
	if (scene == nullptr) {
		std::cerr << "Could not load model from " << filename << std::endl;
		return nullptr;
	}
	aiMesh* mesh = scene->mMeshes[0];
	
	newModel->loadVertices(mesh, newModel);
	newModel->loadIndices(mesh, newModel, physWorld, worldCtx);
	newModel->material = newModel->loadMaterials(scene);
	newModel->initialised = false;
	
	//Now save this model for later in case we need to use it again
	loadedModels[filename] = newModel;
	
	return newModel;
}

void PhysicsModel::loadVertices(aiMesh* mesh, PhysicsModel* newModel) {
	//Add all our vertices for openGL
	for (unsigned vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
		Vertex newVert = loadVerticesExtended(mesh, vertexIndex);
		newModel->_vertices.push_back(newVert);
	}
}

void
PhysicsModel::loadIndices(aiMesh* mesh, PhysicsModel* newModel, PhysicsWorld* physWorld, PhysicsWorld::Context* ctx) {
	static int phsyics_world_model_index = 0;
	std::string indexName = std::to_string(phsyics_world_model_index);
	phsyics_world_model_index++;
	//Bullet needs this info
	btVector3 triArray[3];


	// Shapes are: (1 - Sphere, 2 - Box, 3 - Cylinder, 4 - Plane) : After are meshes (Static/Dynamics)
	// If not a default shape, load indices for triangle mesh
	if(ctx->shape > 4 || ctx->shape <= 0)
	{
		objTriMesh = new btTriangleMesh();
		//Now our indices
		for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];

			for (unsigned vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
				newModel->_indices.push_back(face.mIndices[vertexIndex]);

				aiVector3D position = mesh->mVertices[face.mIndices[vertexIndex]];
				triArray[vertexIndex] = btVector3(position.x, position.y, position.z);
			}
			objTriMesh->addTriangle(triArray[0], triArray[1], triArray[2]);
		}
	}
	else
	{
		for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];

			for (unsigned vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
				newModel->_indices.push_back(face.mIndices[vertexIndex]);
			}
		}
	}

	
	// create the mesh and add it to the world
	// btCollisionShape *triMeshShape = new btBvhTriangleMeshShape(objTriMesh, true);
	// TODO: Make sure triangle mesh is being used correctly.
	
	rigidBodyIndex = physWorld->createObject(indexName, objTriMesh, ctx);
}

int PhysicsModel::getRigidBodyIndex() {
	return rigidBodyIndex;
}

#endif /* PHYSICS_MODEL */