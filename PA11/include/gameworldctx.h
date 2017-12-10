//
// Created by matt on 10/27/17.
//

#ifndef GAMEWORLDCTX_H
#define GAMEWORLDCTX_H

#include <vector>
#include "object.h"

class Object;

#define MODE_NONE      0 //Nothing is currently happening - only in between games
#define MODE_PLACE_CUE 1 //Someone should be placing the cue ball
#define MODE_TAKE_SHOT 2 //Someone is taking their shot
#define MODE_WAIT_NEXT 3 //We're waiting for the balls to come to rest and for the next turn to begin

// A safe space to store the config of our main world
namespace GameWorld {
	struct ctx {
//        Object *surfacePlane;
//        bool surfaceIsDynamic = false;
		std::vector<Object*> worldObjects;
		std::vector<int> ballSolids;
		std::vector<int> ballStripes;
		bool sunk[16];
		bool oob[16];  //out of bounds
		int cueBall;
		int eightBall;
		bool isNextShotOK = true;
		bool isTurnChange = true;
		bool isGameOver = false;
		bool isPlayer1 = true;
		bool isPlayer1Stripes = false;
		bool isPlayer1Solids = false;
		bool isPlayer1Win = false;
		bool isPlayer1Loss = false;
		bool turnSwapped = true;
		
		int mode = MODE_NONE;
		
		int kMod = -1; //For which side of the table we are placing the cue ball onto
	};
}

#endif //AMEWORLDCTX_H
