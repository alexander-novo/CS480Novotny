#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <cmath>
#include "graphics_headers.h"
#include "model.h"
#include "physics_model.h"
#include "shader.h"
#include "Menu.h"


class Menu;

class Object {
	public:
		//The characteristics of a particular planet that can be changed at any time
		struct Context {
            bool isDynamic;
			std::string name = "Object";
			std::string vertexShader;
			std::string fragmentShader;

			PhysicsModel* model = nullptr;
			Texture* texture = nullptr;
			Texture* altTexture = nullptr;
			Texture* normalMap = nullptr;
			Texture* specularMap = nullptr;
			Shader* shader;
			
			bool isLightSource = false;
            int xLoc = 0;
            int yLoc = 0;
            int zLoc = 0;
			int shape = 0;
			float mass = 0;

            bool hasPhysics = true;
            btRigidBody * physicsBody;
		};
		
		Object(const Context &ctx);
		
		~Object();
		
		//Initialises the planet's model and textures for OpenGL
		void Init_GL();
		//Updates the physics for the planet
		void Update(float dt);
		//Renders the planet on the screen
		void Render(float lightPower, GLuint shadowMap) const;
		void renderShadow(Shader* shadowShader) const;

		//Returns the current model matrix of this planet
		const glm::mat4& GetModel() const;
		
		//The current properties of this planet
		Context ctx;
		//The properties the planet started with, for reset purposes
		const Context originalCtx;
		
		//Where in the world the planet is
		const glm::vec3& position;

		static glm::mat4* viewMatrix;
		static glm::mat4* projectionMatrix;
		
		//The shader that every orbit should use
		static Shader* orbitShader;
		
		//Keeps track of what to shift every planet's position by (to keep what we're looking at at the center)
		static glm::vec3 const * globalOffset;

		static Menu* menu;
	
	protected:
		//OpenGL information for rendering
		glm::mat4 modelMat;
		
		glm::vec3 _position;
		
		bool doOffset;
};

#endif /* OBJECT_H */
