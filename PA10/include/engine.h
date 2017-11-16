#ifndef ENGINE_H
#define ENGINE_H

#include <sys/time.h>
#include <assert.h>
#include <random>

#include "window.h"
#include "graphics.h"
#include "Menu.h"
#include "gameworldctx.h"
// ballcount variable
#include "physics_world.h"

#define ENGINE_NAME_DEFAULT "Pinball"
#define ENGINE_WIDTH_DEFAULT 800
#define ENGINE_HEIGHT_DEFAULT 600
#define ENGINE_FULLSCREEN_DEFAULT false

class Engine {
	public:
		struct Context {
			std::string name = ENGINE_NAME_DEFAULT;
			int width = ENGINE_WIDTH_DEFAULT;
			int height = ENGINE_HEIGHT_DEFAULT;
			bool fullscreen = ENGINE_FULLSCREEN_DEFAULT;
			
			PhysicsWorld *physWorld;
			GameWorld::ctx *gameWorldCtx;
			int leftPaddleIndex = -1;
			int rightPaddleIndex = -1;
			int plungerIndex = -1;
			int doorIndex = -1;
			
			std::vector<Graphics::LightContext*>* lights = nullptr;
		};
		
		Engine(const Context &ctx);
		
		~Engine();
		
		//Initialise OpenGL
		//Call after starting OpenGL
		bool Initialize();
		
		//Run the actual program
		void Run();
		
		//Get number of milliseconds since last frame
		unsigned int getDT();
		//Get the current time in milliseconds
		long long GetCurrentTimeMillis();

		// Start a new game
		void NewGame();
		
		//Read-only
		const int& windowWidth;
		const int& windowHeight;
		
		const Context& ctx;
	
	private:
		
		Context _ctx;
		Window *m_window;
		SDL_Event m_event;
		
		Graphics *m_graphics = nullptr;
		unsigned int m_DT;
		long long m_currentTimeMillis;
		bool m_running;

		Menu *m_menu;
		Menu *score_menu;
		
		bool mouseDown;
		
		//Handle keyboard controls
		void Keyboard(unsigned dt);
		//Handle other events (mouse, etc.)
		void eventHandler(unsigned dt);

		// Keeps track of timer powering up
		int plungerTimer = 100;
		bool plungerHit = false;
};

#endif // ENGINE_H
