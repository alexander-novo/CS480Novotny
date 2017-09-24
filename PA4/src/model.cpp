//
// Created by alex on 9/20/17.
//

#include "model.h"

Model* Model::load(std::string filename) {
	Model* newModel = new Model();
	std::string line;
	
	static std::unordered_map<std::string, Model*> loadedModels;
	
	if(loadedModels.find(filename) != loadedModels.end()) {
		return loadedModels[filename];
	}
	
	std::cout << "loading " << filename << "..." << std::endl;
	
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
			Vertex newVertex = {{0.0, 0.0, 0.0}, {((float) rand()) / RAND_MAX, ((float) rand()) / RAND_MAX, ((float) rand()) / RAND_MAX}};
			inFile >> newVertex.vertex.x;
			inFile >> newVertex.vertex.y;
			inFile >> newVertex.vertex.z;

			newModel->_vertices.push_back(newVertex);
		} else if(line == "f") {
			unsigned int faceVertex;
			std::string nextArg;
			
			for(int i = 0; i < 3; i++) {
				inFile >> nextArg;
				faceVertex = strtol(nextArg.c_str(), NULL, 10);
				newModel->_indices.push_back(faceVertex);
			}
		} else {
			getline(inFile, line);
		}
		
		
	}
	
	//Debug Code
	/*std::cout << "Vertex 1: " << newModel->_vertices[0].vertex.x << ", " << newModel->_vertices[0].vertex.y << ", " << newModel->_vertices[0].vertex.z << std::endl;
	std::cout << "Vertices: " << newModel->_vertices.size() << std::endl;
	std::cout << "Faces: " << newModel->_indices.size() << std::endl;*/
	
	for (unsigned int &Indice : newModel->_indices) {
		Indice = Indice - 1;
	}
	
	loadedModels[filename] = newModel;
	
	return newModel;
}

Model::Model(){}