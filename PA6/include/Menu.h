#ifndef TUTORIAL_MENU_H
#define TUTORIAL_MENU_H

#include <map>

#include "window.h"
#include "object.h"

#define MIN_TIME_SCALE -1000.0f
#define MAX_TIME_SCALE  1000.0f
#define MIN_MOVE_SCALE  0.0f
#define MAX_MOVE_SCALE  5.0f

#define CLOSE_SCALE 0.43f

class Menu {
	public:
		//All the different options the menu changes
		struct Options {
			int planetSelector;   //Which planet is currently selected in the drop down
			int numPlanets;       //Keeps track of the number of planets in the drop down
			int lookingAt = 0;    //Which planet our camera should be looking at
			float zoom = 1.0;     //Keeps track of how far our camera should be away from the planet we're looking at
			float rotation = 0.0; //Keeps track of our camera rotating around whatever we're looking at
			float scale = 1.0;    //Keep track of the current distance scale. Applied as an exponential modifier
		};
		
		Menu(Window& window, Object& root);
		
		//Returns a planet based on its index in the drop down
		Object* getPlanet(int index) const;
		
		//Add stuff to the menu and check if anything has changed since last time
		void update(int dt);
		//Draw everything
		void render();
		
		//Set zoom level
		void setZoom(float zoom);
		//Set rotation amount
		void setRotation(float rotation);
		
		//Read-only menu options
		const Options& options;
	private:
		//Take all of the planets and add them into a drop-down formatted string for ImGUI
		//root is where we start (and will be at the top of the list), pre is the prefix for each item in the list
		//and numSatellites keeps track of the number of planets added to the list
		void buildSatelliteList(Object& root, std::string pre, int& numSatellites);
		
		void updateScale(int dt);
		
		//Keep track of where we're rendering and the sun
		Window& window;
		Object& root;
		
		Options _options;
		
		//Keep track of what we should be changing our scale to
		int scaleTo = 1;
		
		//Drop down menu string
		std::string satelliteList;
		//Map which keeps track of which position on the drop down list is which planet (so we can find it later)
		std::map<int, Object*> satelliteMap;
};

#endif //TUTORIAL_MENU_H
