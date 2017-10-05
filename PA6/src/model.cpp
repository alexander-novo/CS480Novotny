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
	
	//Add all our vertices
	for(unsigned vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
		aiVector3D& vertex = mesh->mVertices[vertexIndex];
		aiVector3D& normal = mesh->mNormals[vertexIndex];
		// Guard against objects with no UV
		if(mesh->mTextureCoords[0])
		{
			aiVector3D& uv = mesh->mTextureCoords[0][vertexIndex];	
			newModel->_vertices.push_back({{vertex.x, vertex.y, vertex.z}, {uv.x, uv.y}, {normal.x, normal.y, normal.z}});
		}
		else
		{
			newModel->_vertices.push_back({{vertex.x, vertex.y, vertex.z}, {0, 0}, {normal.x, normal.y, normal.z}});
		}
		
		
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

Texture* Texture::load(std::string filename) {
	static std::unordered_map<std::string, Texture*> loadedTextures;
	Magick::Image m_Image;
	Magick::Blob m_Blob;
	
	//If we already have the texture loaded, don't load it again
	if(loadedTextures.find(filename) != loadedTextures.end()) {
		return loadedTextures[filename];
	}
	
	//Load our texture with ImageMagick
	try {
		m_Image.read(filename);
		m_Image.write(&m_Blob, "RGBA");
	} catch(Magick::Error& err) {
		std::cout << "Could not load texture \"" << filename <<"\": " << err.what() << std::endl;
		return nullptr;
	}
	
	Texture* newTex = new Texture();
	
	//set up the texture with OpenGL
	glGenTextures(1, &newTex->m_textureObj);
	glBindTexture(GL_TEXTURE_2D, newTex->m_textureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Image.columns(), m_Image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Blob.data());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Now add it onto our list
	loadedTextures[filename] = newTex;
	
	return newTex;
}

void Texture::bind() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureObj);
}

Texture::Texture(){}