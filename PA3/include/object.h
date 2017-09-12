#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <cmath>
#include "graphics_headers.h"

class Object {
	public:
		struct Context {
			float timeScale = 1.0;
			float moveScale = 1.0;
			float spinScale = 1.0;
			float orbitDistance = 10.0;
			
			float scale = 1.0;
			
			int moveDir = -1;
			int spinDir = -1;
			
			std::string name = "Planet";
		};
		
		Object(const Context &ctx);
		
		~Object();
		
		void Update(float dt, const glm::mat4 &parentModel);
		
		void Render(GLint &modelLocation);
		
		glm::mat4 GetModel();
		
		Context ctx;
		const Context& originalCtx;
		std::vector<Object> children;
	
	private:
		glm::mat4 model;
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		GLuint VB;
		GLuint IB;
		
		Context _originalCtx;
		
		struct Time {
			float spin, move;
		} time;
};

#endif /* OBJECT_H */
