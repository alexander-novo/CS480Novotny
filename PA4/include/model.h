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
		static Model* load(std::string filename);
		
		std::vector<Vertex> _vertices;
		std::vector<unsigned int> _indices;
		
	private:
		Model();
		
		
};

#endif //TUTORIAL_MODEL_H
