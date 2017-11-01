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

class Graphics {
	public:
		Graphics(float lightPower, Menu& menu, const int& w, const int& h, PhysicsWorld *pW, GameWorld::ctx *gwc);
		~Graphics();
		
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
		
		//How much light the sun should give off
		float lightPower;

		const int& windowWidth;
		const int& windowHeight;
		
		PhysicsWorld* physWorld;
		GameWorld::ctx *gameWorldCtx;
		
		GLuint pickDepthBuffer;
		GLuint pickBuffer;
		GLuint pickTexture;
		
		Shader* pickShader;
};

#endif /* GRAPHICS_H */
