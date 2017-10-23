#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <cmath>
#include "graphics_headers.h"
#include "model.h"
#include "shader.h"
#include "Menu.h"

#define DRAW_NO_ORBITS     0
#define DRAW_ALL_ORBITS    1
#define DRAW_PLANET_ORBITS 2
#define DRAW_MOON_ORBITS   3

class Menu;

class Object {
	public:
		//The characteristics of a particular planet that can be changed at any time
		struct Context {
			float timeScale = 1.0;
			float moveScale = 1.0;
			float spinScale = 1.0;
			float orbitDistance = 10.0;
			float orbitTilt = 0.0;
			float axisTilt = 0.0;
			
			float scale = 1.0;
			
			int moveDir = 1;
			int spinDir = -1;

			bool hasRings = false;
			
			std::string name = "Planet";
			std::string vertexShader;
			std::string fragmentShader;
			
			Model* model;
			Model* ringsModel = nullptr;
			Texture* ringsTexture = nullptr;
			Texture* texture = nullptr;
			Texture* altTexture = nullptr;
			Texture* normalMap = nullptr;
			Texture* specularMap = nullptr;
			Shader* shader;
			
			bool isLightSource = false;
			
			float scaleMultiplier; //Used to determine what to multiply a planet's scale by in close scale
		};
		
		Object(const Context &ctx, Object* parent);
		
		~Object();
		
		//Initialises the planet's model and textures for OpenGL
		void Init_GL();
		//Updates the physics for the planet
		void Update(float dt, float scaleExp, bool drawOrbits);
		//Renders the planet on the screen
		void Render(float lightPower, unsigned drawOrbits, GLuint shadowMap) const;
		void renderShadow(Shader* shadowShader) const;
		//Adds a satellite to this planet with the specified properties
		Object& addChild(const Context& ctx);
		//Gets the number of satellites
		unsigned long getNumChildren() const;
		
		//Returns the current model matrix of this planet
		const glm::mat4& GetModel() const;
		
		//The current properties of this planet
		Context ctx;
		//The properties the planet started with, for reset purposes
		const Context originalCtx;
		//Pointer to the planet this one is orbiting - NULL for sun-type objects
		Object* const parent;
		
		//Where in the world the planet is
		const glm::vec3& position;
		
		//Returns true if the object isn't orbiting the sun (or is the sun itself)
		bool isMoon() const;
		//Returns how many things this object is orbiting
		unsigned orbitDepth() const;
		//Returns true if this object is orbiting the given object
		bool isMoonOf(Object const *) const;
		
		//Returns a reference to a satellite
		Object& operator[](int index);
		
		static glm::mat4* viewMatrix;
		static glm::mat4* projectionMatrix;
		
		//The shader that every orbit should use
		static Shader* orbitShader;
		
		//Keeps track of what to shift every planet's position by (to keep what we're looking at at the center)
		static glm::vec3 const * globalOffset;
		
		static Menu* menu;
	
	protected:
		//Timing information for keeping track of orbits and such
		struct Time {
			float spin, move;
		} time;
		
		//OpenGL information for rendering
		glm::mat4 modelMat;

		GLuint VB;
		GLuint IB;
		
		//Keep track of buffers for orbit vertices
		//Pair of # vertices and actual buffer
		std::pair<unsigned, GLuint> OB;
		std::pair<unsigned, GLuint> OB_REAL_FAR;
		std::pair<unsigned, GLuint> OB_REAL_ZOOMED;
		std::pair<unsigned, GLuint> OB_CLOSE;
		
		//Which one we're actually using
		std::pair<unsigned, GLuint> OB_cur;
		
		struct OrbitInfo {
			glm::vec3 lastParentPos;
			float lastScale;
			bool lastFocus;
		} orbitInfo;
		
		glm::vec3 _position;
		
		//List of satellites
		std::vector<Object*> _children;
		
		bool doOffset;
		
		//Determine which orbit buffer to use, and generate more if neccesary
		void updateOrbit(float scaleExp);
		//Generates orbit vertices and stuffs them into the given buffer
		void calcOrbit(float scaleExp, unsigned numDashes, std::pair<unsigned, GLuint>& buffer);
		//Render orbit dashes
		void drawOrbit() const;
		//Render rings (if they exist)
		void RenderRings(float lightPower) const;
};

#endif /* OBJECT_H */
