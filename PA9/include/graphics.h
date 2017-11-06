#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "shader.h"
#include "object.h"
#include "Menu.h"
#include "physics_world.h"
#include "camera.h"
#include "gameworldctx.h"

#define LIGHT_POINT 1
#define LIGHT_SPOT  2

class Graphics {
	public:
		struct LightContext {
			int type;           //Point or spot light
			glm::vec3 position; //Where in the world the light is
			glm::vec3 const * pointing; //Where the light is pointing, if spot light
			float strength;     //How bright
			float angle;        //How wide of a cone - for spot lights
		};
		
		Graphics(Menu& menu, const int& w, const int& h, PhysicsWorld *pW, GameWorld::ctx *gwc);
		~Graphics();
		
		void addLight(const LightContext& light);
		
		//Initialise OpenGL
		bool Initialize(int width, int height);
		
		//Update physics and models
		void Update(unsigned int dt);
		
		//Render models
		void Render();
		
		//Return pointer to vector of objects
		vector<Object *> *getObject();
		
		Object* getObjectOnScreen(int x, int y);

		Camera * getCamView();
	
	private:
		
		struct PixelInfo {
			float r;
			float g;
			float b;
		};

		// The camera view
		Camera *camView = nullptr;
		std::string ErrorString(GLenum error);

		Menu& m_menu;
		
		//Render pass for mouse picking
		void renderPick();

		const int& windowWidth;
		const int& windowHeight;
		
		PhysicsWorld* physWorld;
		GameWorld::ctx *gameWorldCtx;
		
		vector<LightContext> spotLights;
		/*vector<float> spotLightPositions;
		vector<float> spotLightDirections;
		vector<float> spotLightAngles;
		vector<float> spotLightStrengths;*/
		
		GLuint pickDepthBuffer;
		GLuint pickBuffer;
		GLuint pickTexture;
		
		Shader* pickShader;
};

#endif /* GRAPHICS_H */
