//
// Created by alex on 9/20/17.
//

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "graphics_headers.h"

#ifndef TUTORIAL_MODEL_H
#define TUTORIAL_MODEL_H

class Model {
	public:
		struct Material {
			glm::vec4 ambient = {0.2, 0.2, 0.2, 1.0};  //Ka
			glm::vec4 diffuse = {0.8, 0.8, 0.8, 1.0};  //Kd
			glm::vec4 specular = {0.0, 0.0, 0.0, 1.0}; //Ks
			
			float shininess = 0.0f;                    //Ns
		};
		static Model* load(std::string filename);
		static void loadMaterials(Model* model, std::string filename);
		
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;
		
		Material material;
		
	private:
		Model();
		
		std::unordered_map<std::string, Material> materialList;
};

#endif //TUTORIAL_MODEL_H
