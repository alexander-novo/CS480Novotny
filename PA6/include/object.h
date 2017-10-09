#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <cmath>
#include "graphics_headers.h"
#include "model.h"
#include "shader.h"

class Object {
	public:
		//The characteristics of a particular planet that can be changed at any time
		struct Context {
			float timeScale = 1.0;
			float moveScale = 1.0;
			float spinScale = 1.0;
			float orbitDistance = 10.0;
			float orbitTilt = 0.0;
			float axisTilt = 0.0;
			
			float scale = 1.0;
			
			int moveDir = -1;
			int spinDir = -1;
			
			std::string name = "Planet";
			std::string vertexShader;
			std::string fragmentShader;
			
			Model* model;
			Texture* texture = nullptr;
			Texture* altTexture = nullptr;
			Texture* normalMap = nullptr;
			Shader* shader;
			
			bool isLightSource = false;
			
			float scaleMultiplier; //Used to determine what to multiply a planet's scale by in close scale
		};
		
		Object(const Context &ctx, Object* parent);
		
		~Object();
		
		//Initialises the planet's model and textures for OpenGL
		void Init_GL();
		//Updates the physics for the planet
		void Update(float dt, const glm::mat4 &parentModel, float scaleExp, bool drawOrbits);
		//Renders the planet on the screen
		void Render(float lightPower, bool drawOrbits) const;
		//Adds a satellite to this planet with the specified properties
		Object& addChild(const Context& ctx);
		//Gets the number of satellites
		unsigned long getNumChildren() const;
		
		//Returns the current model matrix of this planet
		const glm::mat4& GetModel() const;
		
		//The current properties of this planet
		Context ctx;
		//The properties the planet started with, for reset purposes
		const Context originalCtx;
		//Pointer to the planet this one is orbiting - NULL for sun-type objects
		Object* const parent;
		
		//Returns a reference to a satellite
		Object& operator[](int index);
		
		static glm::mat4* viewMatrix;
		static glm::mat4* projectionMatrix;
		
		static Shader* orbitShader;
	
	private:
		//Timing information for keeping track of orbits and such
		struct Time {
			float spin, move;
		} time;
		
		//OpenGL information for rendering
		glm::mat4 modelMat;

		GLuint VB;
		GLuint IB;
		GLuint OB;
		
		struct OrbitInfo {
			glm::vec4 lastParentPos;
			float lastScale;
			
		} orbitInfo;
		
		//List of satellites
		std::vector<Object*> _children;
		
		std::vector<glm::vec3> orbitVertices;
		
		void updateOrbit(const glm::mat4& parentModel, float scaleExp);
		void drawOrbit() const;
};

#endif /* OBJECT_H */
