#ifndef TUTORIAL_MENU_H
#define TUTORIAL_MENU_H

#include <map>

#include "window.h"
#include "object.h"

#define MIN_TIME_SCALE -5.0f
#define MAX_TIME_SCALE  5.0f
#define MIN_MOVE_SCALE  0.0f
#define MAX_MOVE_SCALE  5.0f

class Menu {
	public:
		struct Options {
			int planetSelector; //Which planet is currently selected in the drop down
			int numPlanets; //Keeps track of the number of planets in the drop down
			int lookingAt = -1; //Which planet our camera should be looking at
		};
		
		Menu(Window& window, Object& root);
		
		Object* getPlanet(int index) const;
		
		void update();
		void render();
		
		const Options& options;
	private:
		void buildSatelliteList(Object& root, std::string pre, int& numSatellites);
		
		Window& window;
		Object& root;
		
		Options _options;
		
		std::string satelliteList;
		std::map<int, Object*> satelliteMap;
};

#endif //TUTORIAL_MENU_H
