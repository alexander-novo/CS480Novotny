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
			bool isPaddle = false;
			std::string name = "Object";
			std::string vertexShader;
			std::string fragmentShader;

			PhysicsModel* model = nullptr;
			Texture* texture = nullptr;
			Texture* altTexture = nullptr;
			Texture* normalMap = nullptr;
			Texture* specularMap = nullptr;
			Shader* shader;
			Shader* altShader;
			
            int xLoc = 0;
            int yLoc = 0;
            int zLoc = 0;
			float scaleX = 1;
			float scaleY = 1;
			float scaleZ = 1;
			int shape = 0;
			float mass = 0;
			
			float scale = 1.0f;
			
			int id = 0;

            bool hasPhysics = true;
            btRigidBody * physicsBody;

			int leftPaddleIndex = -1;
			int rightPaddleIndex = -1;
			int plungerIndex = -1;
		};
		
		Object(const Context &ctx);
		
		~Object();
		
		//Initialises the planet's model and textures for OpenGL
		void Init_GL(std::unordered_map<std::string, std::string> const * dictionary = nullptr);
		//Updates the physics for the planet
		void Update(float dt);
		//Renders the planet on the screen
		void Render() const;
		void RenderID(Shader* shader) const;

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

		static Menu* menu;
	
	protected:
		//OpenGL information for rendering
		glm::mat4 modelMat;
		
		glm::vec3 _position;
		
		bool doOffset;
		
		static int idCounter;
};

#endif /* OBJECT_H */
