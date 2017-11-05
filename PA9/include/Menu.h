#ifndef TUTORIAL_MENU_H
#define TUTORIAL_MENU_H

#include <map>

#include "window.h"
#include "object.h"

class Object;

class Menu {
	public:
		//All the different options the menu changes
		struct Options {
			float zoom = 1.0;     //Keeps track of how far our camera should be away from the planet we're looking at
			float rotation = 0.0; //Keeps track of our camera rotating around whatever we're looking at
			bool drawShadows = false;
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
		
		//Read-only menu options
		const Options& options;
	private:
		//Keep track of where we're rendering and the sun
		Window& window;
		
		Options _options;
};

#endif //TUTORIAL_MENU_H
