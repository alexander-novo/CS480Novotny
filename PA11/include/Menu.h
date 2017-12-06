#ifndef TUTORIAL_MENU_H
#define TUTORIAL_MENU_H

#include <map>

#include "graphics_headers.h"
#include "window.h"

#define MENU_SHADER_VERTEX   0
#define MENU_SHADER_FRAGMENT 1

#define MENU_SHADOWS_NONE 0
#define MENU_SHADOWS_LOW  1024
#define MENU_SHADOWS_MED  2048
#define MENU_SHADOWS_HIGH 4096

#define MENU_OPTIONS_INDENT 16.0f

class Object;

class Menu {
	public:
		//All the different options the menu changes
		struct Options {
			float zoom = 20.0;     //Keeps track of how far our camera should be away from the planet we're looking at
			float rotation = 0.0; //Keeps track of our camera rotating around whatever we're looking at
			float elevation = 45;
			bool shouldClose = false; //Whether or not we should exit the program
			int shadowSize = MENU_SHADOWS_LOW; //Size of shadow maps
			bool changedShadowSize = false; //True if shadowSize changed in the last frame
			
			bool paused = false; //Pause simulation
			bool showOptionsMenu = false; //Whether or not the options menu is out right now
			bool showPlayers = true; //Whether or not the player1/player2 menu is currently shown
			bool shouldSwapShaders = false; //Whether the engine should swap shaders this frame
			bool shouldStartNewGame = false;

			int singleBall = 1; //Whether to play with 1 ball or 3
			bool isPlayingMusic = false;
			bool isPlayingSounds = true;
			bool isPlayer1Turn = true;
            bool isPlayer1Stripes = false;
            bool isPlayer1Solids = false;
			bool isPlayer2Turn = false;
			bool playerCheckBoxDisabled = true;
			
			int shaderType = MENU_SHADER_FRAGMENT; //The current shader we should be rendering with
			bool rotateBack = false;
			
			glm::vec3 ambientColor = glm::vec3(0.05, 0.05, 0.05);
		};
		
		Menu(Window& window);
		
		//Add stuff to the menu and check if anything has changed since last time
		void update(int dt, float width, float height);
		//Draw everything
		void render();
		
		//Set zoom level
		void setZoom(float zoom);
		//Set rotation amount
		void setRotation(float rotation);
		void setElevation(float elevation);
		
		void toggleOptionsMenu();
		void swapShaderType();
		void pause();

		// Allow callback to see game mode type
		static int singleBall(int singleBall = -1);
		
		//Read-only menu options
		const Options& options;
	private:
		//Keep track of where we're rendering and the sun
		Window& window;
		
		Options _options;
		
		int prevShaderType = MENU_SHADER_FRAGMENT;
};

#endif //TUTORIAL_MENU_H
