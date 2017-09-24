#include "object.h"

Object::Object(const Context &a, Object* b) : ctx(a), originalCtx(a), parent(b) {
	time.spin = time.move = 0.0;
}

Object::~Object() {
	//Vertices.clear();
	//Indices.clear();
	for (auto &i : _children) {
		delete i;
	}
	_children.clear();
}

void Object::Init_GL() {
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * ctx.model->_vertices.size(), &ctx.model->_vertices[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ctx.model->_indices.size(), &ctx.model->_indices[0], GL_STATIC_DRAW);
	
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
	modelMat= glm::rotate(parentModel, time.move, glm::vec3(0.0, 1.0, 0.0));
	modelMat= glm::translate(modelMat, glm::vec3(ctx.orbitDistance, 0.0, 0.0));
	modelMat= glm::rotate(modelMat, -time.move, glm::vec3(0.0, 1.0, 0.0));
	
	//Update all satellites after moving, so they follow us around
	for (auto &i : _children) {
		i->Update(dt * ctx.timeScale, modelMat);
	}
	
	//Then rotate and scale so the satellites are unaffected
	modelMat= glm::rotate(modelMat, time.spin, glm::vec3(0.0, 1.0, 0.0));
	modelMat= glm::scale(modelMat, glm::vec3(ctx.scale, ctx.scale, ctx.scale));
	
}

const glm::mat4& Object::GetModel() const {
	return modelMat;
}

void Object::Render(GLint &modelLocation) const {
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	
	glDrawElements(GL_TRIANGLES, ctx.model->_indices.size(), GL_UNSIGNED_INT, 0);
	
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
