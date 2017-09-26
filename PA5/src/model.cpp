//
// Created by alex on 9/20/17.
//

#include "model.h"

Model* Model::load(std::string filename) {
	Model* newModel = new Model();
	std::string line;
	//Maps each vertex to a list of normals, which are the normals of the faces attached to it
	std::unordered_map<unsigned, std::vector<glm::vec3>> faceNormals;
	std::vector<glm::vec3> vertexNormals;
	
	static std::unordered_map<std::string, Model*> loadedModels;
	
	if(loadedModels.find(filename) != loadedModels.end()) {
		return loadedModels[filename];
	}
	
	std::ifstream inFile(filename);
	if(!inFile.is_open()) return NULL;
	
	while(true) {
		getline(inFile, line, ' ');
		if(inFile.eof()) break;
		
		while(std::isspace(line[0])) {
			line.erase(0, 1);
		}
		
		//std::cout << "Looking at: " << line << std::endl;
		
		if(line == "#") {
			//Skip line - comment
			getline(inFile, line);
		} else if(line == "v") {
			//New vertex - add it into the list
			//v # # #
			Vertex newVertex = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
			inFile >> newVertex.vertex.x;
			inFile >> newVertex.vertex.y;
			inFile >> newVertex.vertex.z;

			newModel->_vertices.push_back(newVertex);
		} else if (line == "vn") {
			//vertex normals - to be used by faces later
			//vn # # #
			glm::vec3 newVector;
			inFile >> newVector.x;
			inFile >> newVector.y;
			inFile >> newVector.z;
			
			vertexNormals.push_back(newVector);
		} else if(line == "f") {
			//New face
			//f #/#/# #/#/# #/#/#
			unsigned int faceVertex[3];
			std::string nextArg;
			char* numEnding;
			unsigned vertexIndex;
			
			for (unsigned int &i : faceVertex) {
				//Just take the first number, we don't need to know texture mapping or normals
				inFile >> nextArg;
				i = strtol(nextArg.c_str(), &numEnding, 10);
				newModel->_indices.push_back(i);
				
				//Scratch that I actually need the third number
				strtol(numEnding + 1, &numEnding, 10); //just in case of the second number
				vertexIndex = strtol(numEnding + 1, &numEnding, 10) - 1;
				
			}
			
			//Get the face normal and add it to the list
			for (unsigned int i : faceVertex) {
				faceNormals[i - 1].push_back(vertexNormals[vertexIndex]);
			}
		} else if(line == "mtllib") {
			//Material file
			//mtllib <filename>
			inFile >> line;
			loadMaterials(newModel, "models/" + line);
		} else if(line == "usemtl") {
			//Use this material for the next few faces
			//usemtl <material name>
			std::string mtlName;
			inFile >> mtlName;
			
			if(newModel->materialList.find(mtlName) == newModel->materialList.end())  {
				std::cout << "Material \"" << mtlName << "\" not found!" << std::endl;
				return NULL;
			}
			
			newModel->material = newModel->materialList[mtlName];
		} else {
			getline(inFile, line);
		}
	}
	
	for (unsigned int &Indice : newModel->_indices) {
		Indice = Indice - 1;
	}
	
	for(unsigned i = 0; i < newModel->_vertices.size(); i++) {
		glm::vec3 sum = {0.0, 0.0, 0.0};
		for(const glm::vec3& faceNormal : faceNormals[i]) {
			sum += faceNormal;
		}
		newModel->_vertices[i].normal = glm::normalize(sum);
	}
	
	
	loadedModels[filename] = newModel;
	
	return newModel;
}

void Model::loadMaterials(Model* model, std::string filename) {
	Model::Material newMaterial;
	std::string line;
	std::string currentName;
	
	std::ifstream inFile(filename);
	if(!inFile.is_open()) {
		std::cout << "Could not open material file \"" << filename << "\"" << std::endl;
		return;
	}
	
	while(true) {
		getline(inFile, line, ' ');
		if(inFile.eof()) break;
		
		while(std::isspace(line[0])) {
			line.erase(0, 1);
		}
		
		if(line == "#") {
			//Skip line - comment
			getline(inFile, line);
		} else if(line == "newmtl") {
			//New material
			//newmtl <name>
			if(currentName != "") {
				model->materialList[currentName] = newMaterial;
			}
			
			inFile >> line;
			currentName = line;
		} else if(line == "Ns") {
			//Specular exponent / shininess
			//Ns #
			float shininess;
			
			inFile >> line;
			shininess = strtod(line.c_str(), NULL);
			
			newMaterial.shininess = shininess;
		} else if(line == "Ka") {
			//Ambient
			//Ka # # #
			inFile >> newMaterial.ambient.r;
			inFile >> newMaterial.ambient.g;
			inFile >> newMaterial.ambient.b;
		} else if(line == "Kd") {
			//Diffuse
			//Kd # # #
			inFile >> newMaterial.diffuse.r;
			inFile >> newMaterial.diffuse.g;
			inFile >> newMaterial.diffuse.b;
		} else if(line == "Ks") {
			//Specular
			//Ks # # #
			inFile >> newMaterial.specular.r;
			inFile >> newMaterial.specular.g;
			inFile >> newMaterial.specular.b;
		} else if(line == "d") {
			//Opacity
			//d #
			float opacity;
			
			inFile >> opacity;
			
			newMaterial.ambient.a = opacity;
			newMaterial.diffuse.a = opacity;
			newMaterial.specular.a = opacity;
		} else if(line == "Tr") {
			//Transparency
			//Tr #
			float opacity;
			
			inFile >> opacity;
			
			newMaterial.ambient.a = 1.0f - opacity;
			newMaterial.diffuse.a = 1.0f - opacity;
			newMaterial.specular.a = 1.0f - opacity;
		} else {
			getline(inFile, line);
		}
	}
	
	if(currentName != "") {
		model->materialList[currentName] = newMaterial;
	}
}

Model::Model(){}