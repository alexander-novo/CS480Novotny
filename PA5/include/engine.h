#ifndef ENGINE_H
#define ENGINE_H

#include <sys/time.h>
#include <assert.h>

#include "window.h"
#include "graphics.h"
#include "Menu.h"

#define ENGINE_NAME_DEFAULT "Tutorial Window Name"
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
			
			std::string vertex = "";
			std::string fragment = "";
			
			float lightStrength = 1.0f;
		};
		
		Engine(const Context &ctx, Object* sun);
		
		~Engine();
		
		bool Initialize();
		
		void Run();
		
		void Keyboard();
		
		unsigned int getDT();
		
		long long GetCurrentTimeMillis();
		
		//Read-only
		const int& windowWidth;
		const int& windowHeight;
	
	private:
		// Window related variables
		Window *m_window;
		string m_WINDOW_NAME;
		int m_WINDOW_WIDTH;
		int m_WINDOW_HEIGHT;
		bool m_FULLSCREEN;
		float m_light;
		SDL_Event m_event;
		
		Graphics *m_graphics;
		unsigned int m_DT;
		long long m_currentTimeMillis;
		bool m_running;
		
		Object* m_cube;
		Menu *m_menu;
		
		bool mouseDown;
		
		std::string m_vertexShader, m_fragmentShader;
};

#endif // ENGINE_H
