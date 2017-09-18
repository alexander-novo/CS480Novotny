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
		
		struct Time {
			float spin, move;
		} time;
		
		Object(const Context &ctx, Object* parent);
		
		~Object();
		
		void Init_GL();
		void Update(float dt, const glm::mat4 &parentModel);
		void Render(GLint &modelLocation) const;
		Object& addChild(const Context& ctx);
		unsigned long getNumChildren() const;
		
		const glm::mat4& GetModel() const;
		
		Context ctx;
		const Context& originalCtx;
		Object* const parent;
		
		Object& operator[](int index);
	
	private:
		glm::mat4 model;
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		GLuint VB;
		GLuint IB;
		
		Context _originalCtx;
		std::vector<Object*> _children;
};

#endif /* OBJECT_H */
