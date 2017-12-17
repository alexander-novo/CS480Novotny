#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

#include "imgui_impl_sdl_gl3.h"

using namespace std;

#define FLIPPER_SOUND "sounds/flipper.wav"
#define BUMPER_SOUND "sounds/bumper.wav"
#define BGMUSIC_SOUND "sounds/music.mp3"
#define LAUNCHER_SOUND "sounds/launch.wav"
#define BUMPER_ALT_SOUND "sounds/explosion.wav"

class Window
{
	public:
		Window();
		~Window();
		bool Initialize(const string &name, int* width, int* height);
		void Swap();
		void PlayMusic(bool isPlaying);
		bool isPlayingMusic = true;

		SDL_Window* getSDL_Window() const;

		// Sound files to be loaded with SDL
		static Mix_Chunk *bumperSound;
		static Mix_Chunk *flipperSound;
		static Mix_Music *bgMusicSound;
		static Mix_Chunk *launcherSound;
		static Mix_Chunk *explodeSound;


	private:
		SDL_Window* gWindow;
		SDL_GLContext gContext;
};

#endif /* WINDOW_H */
