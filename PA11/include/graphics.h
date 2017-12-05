#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>

#include "graphics_headers.h"
#include "object.h"
#include "shader.h"
#include "Menu.h"
#include "camera.h"
#include "gameworldctx.h"

#define LIGHT_POINT 1
#define LIGHT_SPOT  2

class Graphics {
	public:
		struct LightContext {
			int type;                   //Point or spot light
			glm::vec3 position;         //Where in the world the light is
			glm::vec3 const * pointing; //Where the light is pointing, if spot light
			glm::vec3 color;            //What color the light is
			bool isRainbow = false;     //Whether the color should change over time
			bool isBumperLight = false; //Whether or not the light stays on for only a set period of time
			unsigned timer = 0;         //Timer for color over time
			float strength;             //How bright
			float angle;                //How wide of a cone - for spot lights
		};
		
		Graphics(Menu& menu, const int& w, const int& h, GameWorld::ctx *gwc);
		~Graphics();
		
		void addLight(LightContext* light);
		
		//Initialise OpenGL
		bool Initialize(int width, int height);
		
		//Update physics and models
		void Update(unsigned int dt);
		
		//Render models
		void Render();
		
		//Return pointer to vector of objects
		vector<Object *> *getObject();
		
		Object* getObjectOnScreen(int x, int y, glm::vec3* location = nullptr);
		
		void updateScreenSize(int width, int height);

		Camera * getCamView();
		
		void addGuiBillboard(const glm::vec3& location, Texture* texture);
		
		//DO NOT MODIFY AFTER Initialize() HAS BEEN CALLED
		//Feel free to modify the LightContext objects, though
		vector<LightContext*> spotLights;
	
	private:
		
		struct PixelInfo {
			float r;
			float g;
			float b;
			float a;
		};
		
		std::vector<pair<glm::vec3, Texture*>> billboards;

		// The camera view
		Camera *camView = nullptr;
		std::string ErrorString(GLenum error);

		Menu& m_menu;
		
		//Render pass for mouse picking
		void renderPick();
		//Render pass for shadow mapping
		void renderShadows();
		void renderBillboards();

		const int& windowWidth;
		const int& windowHeight;
		
		GameWorld::ctx *gameWorldCtx;
		
		GLuint pickBuffer;
		GLuint pickTexture;
		
		GLuint spotlightShadowBuffer;
		GLuint spotlightShadowTexture;
		std::vector<glm::mat4> spotlightMatrices; //View-Projection matrices for each light
		
		Shader* pickShader;
		Shader* shadowShader;
};

#endif /* GRAPHICS_H */
