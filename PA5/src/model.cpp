//
// Created by alex on 9/20/17.
//

#include "model.h"

Model* Model::load(std::string filename) {
	static std::unordered_map<std::string, Model*> loadedModels;
	
	//If we already loaded this model before, don't do it again
	if(loadedModels.find(filename) != loadedModels.end()) {
		return loadedModels[filename];
	}

	Model* newModel = new Model();
	
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast);
	aiMesh* mesh = scene->mMeshes[0];

	Magick::Image* m_pImage;
    Magick::Blob m_blob;
	
	//Add all our vertices
	for(unsigned vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
		aiVector3D& vertex = mesh->mVertices[vertexIndex];
		aiVector3D& normal = mesh->mNormals[vertexIndex];
		newModel->_vertices.push_back({{vertex.x, vertex.y, vertex.z}, {normal.x, normal.y, normal.z}});
	}
	
	//Now our indices
	for(unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
		aiFace& face = mesh->mFaces[faceIndex];
		
		for(unsigned vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			newModel->_indices.push_back(face.mIndices[vertexIndex]);
		}
	}
	
	//Now material stuff
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiColor3D ambientColor(0.0f, 0.0f, 0.0f);
	aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
	aiColor3D specularColor(0.0f, 0.0f, 0.0f);
	
	material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
	material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	
	Material modelMaterial;
	modelMaterial.ambient = {ambientColor.r, ambientColor.g, ambientColor.b};
	modelMaterial.diffuse = {diffuseColor.r, diffuseColor.g, diffuseColor.b};
	modelMaterial.specular = {specularColor.r, specularColor.g, specularColor.b};
	
	newModel->material = modelMaterial;
	
	//Now save this model for later in case we need to use it again
	loadedModels[filename] = newModel;
	
	return newModel;
}

Model::Model(){}