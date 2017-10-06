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
	
	if(ctx.texture != nullptr) {
		ctx.texture->initGL();
	}
	
	//Now do the same for all our children
	for(auto& child : _children) {
		child->Init_GL();
	}
}

void Object::Update(float dt, const glm::mat4 &parentModel, float scaleExp) {
	float timeMod = dt / 1000.0f * ctx.timeScale;
	float scale = ctx.scaleMultiplier / pow(ctx.scaleMultiplier, scaleExp) * pow(ctx.scale, scaleExp);
	
	//Update the timer
	time.spin += timeMod * ctx.spinScale * ctx.spinDir;
	time.move += timeMod * ctx.moveScale * ctx.moveDir;
	
	//Move into place
	modelMat= glm::rotate(parentModel, -time.move, glm::vec3(0.0, 1.0, 0.0));
	modelMat= glm::translate(modelMat, glm::vec3(pow(ctx.orbitDistance, scaleExp), 0.0, 0.0));
	modelMat= glm::rotate(modelMat, time.move, glm::vec3(0.0, 1.0, 0.0));
	
	//Update all satellites after moving, so they follow us around
	for (auto &i : _children) {
		i->Update(dt * ctx.timeScale, modelMat, scaleExp);
	}
	
	//Then rotate and scale so the satellites are unaffected
	modelMat= glm::rotate(modelMat, time.spin, glm::vec3(0.0, 1.0, 0.0));
	modelMat= glm::scale(modelMat, glm::vec3(scale, scale, scale));
}

const glm::mat4& Object::GetModel() const {
	return modelMat;
}

void Object::Render(GLint &modelLocation, GLint &ambientLocation, GLint &diffuseLocation, GLint &specularLocation, GLint &sourceLocation, GLint &textureLocation) const {
	//Send our shaders the model matrix
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
	
	//Send the material information
	glUniform3fv(ambientLocation, 1, &ctx.model->material.ambient.r);
	glUniform3fv(diffuseLocation, 1, &ctx.model->material.diffuse.r);
	glUniform3fv(specularLocation, 1, &ctx.model->material.specular.r);
	
	glUniform1i(sourceLocation, ctx.isLightSource);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	//Now send vertices, colors, and normals
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
	
	//Send all our face information
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	
	//If we have a texture, use it
	if(ctx.texture != nullptr) {
		ctx.texture->bind();
		glUniform1i(textureLocation, 0);
	}
	
	//Now draw everything
	glDrawElements(GL_TRIANGLES, ctx.model->_indices.size(), GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Now pass the function down the chain to our satellites
	for (const auto &i : _children) {
		i->Render(modelLocation, ambientLocation, diffuseLocation, specularLocation, sourceLocation, textureLocation);
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
