#include "object.h"

Object::Object(const Context &a, Object* b) : ctx(a), originalCtx(a), parent(b) {
	/*
	  # Blender File for a Cube
	  o Cube
	  v 1.000000 -1.000000 -1.000000
	  v 1.000000 -1.000000 1.000000
	  v -1.000000 -1.000000 1.000000
	  v -1.000000 -1.000000 -1.000000
	  v 1.000000 1.000000 -0.999999
	  v 0.999999 1.000000 1.000001
	  v -1.000000 1.000000 1.000000
	  v -1.000000 1.000000 -1.000000
	  s off
	  f 2 3 4
	  f 8 7 6
	  f 1 5 6
	  f 2 6 7
	  f 7 8 4
	  f 1 4 8
	  f 1 2 4
	  f 5 8 6
	  f 2 1 6
	  f 3 2 7
	  f 3 7 4
	  f 5 1 8
	*/
	
	Vertices = {
			{{1.0f,  -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
			{{1.0f,  -1.0f, 1.0f},  {1.0f, 0.0f, 0.0f}},
			{{-1.0f, -1.0f, 1.0f},  {0.0f, 1.0f, 0.0f}},
			{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
			{{1.0f,  1.0f,  -1.0f}, {1.0f, 1.0f, 0.0f}},
			{{1.0f,  1.0f,  1.0f},  {1.0f, 0.0f, 1.0f}},
			{{-1.0f, 1.0f,  1.0f},  {0.0f, 1.0f, 1.0f}},
			{{-1.0f, 1.0f,  -1.0f}, {1.0f, 1.0f, 1.0f}}
	};
	
	Indices = {
			2, 3, 4,
			8, 7, 6,
			1, 5, 6,
			2, 6, 7,
			7, 8, 4,
			1, 4, 8,
			1, 2, 4,
			5, 8, 6,
			2, 1, 6,
			3, 2, 7,
			3, 7, 4,
			5, 1, 8
	};
	
	// The index works at a 0th index
	for (unsigned int &Indice : Indices) {
		Indice = Indice - 1;
	}
	
	time.spin = time.move = 0.0;
}

Object::~Object() {
	Vertices.clear();
	Indices.clear();
	for (auto &i : _children) {
		delete i;
	}
	_children.clear();
}

void Object::Init_GL() {
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
	
	//Now do the same for all our children
	for(auto& child : _children) {
		child->Init_GL();
	}
}

void Object::Update(float dt, const glm::mat4 &parentModel) {
	float timeMod = dt / 1000.0f * ctx.timeScale;
	
	//Update the timer
	time.spin += timeMod * ctx.spinScale * ctx.spinDir;
	time.move += timeMod * ctx.moveScale * ctx.moveDir;
	
	//Move into place
	model = glm::rotate(parentModel, time.move, glm::vec3(0.0, 1.0, 0.0));
	model = glm::translate(model, glm::vec3(ctx.orbitDistance, 0.0, 0.0));
	model = glm::rotate(model, -time.move, glm::vec3(0.0, 1.0, 0.0));
	
	//Update all satellites after moving, so they follow us around
	for (auto &i : _children) {
		i->Update(dt * ctx.timeScale, model);
	}
	
	//Then rotate and scale so the satellites are unaffected
	model = glm::rotate(model, time.spin, glm::vec3(0.0, 1.0, 0.0));
	model = glm::scale(model, glm::vec3(ctx.scale, ctx.scale, ctx.scale));
	
}

const glm::mat4& Object::GetModel() const {
	return model;
}

void Object::Render(GLint &modelLocation) const {
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	for (const auto &i : _children) {
		i->Render(modelLocation);
	}
}

Object& Object::addChild(const Object::Context& ctx) {
	Object* newPlanet = new Object(ctx, this);
	_children.push_back(newPlanet);
	return *newPlanet;
}

unsigned long Object::getNumChildren() const {
	return _children.size();
}

Object& Object::operator[](int index) {
	if(index >= _children.size()) throw std::out_of_range("Received child index out of range");
	return *_children[index];
}
