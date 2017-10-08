#include "object.h"

glm::mat4* Object::viewMatrix;
glm::mat4* Object::projectionMatrix;

Object::Object(const Context &a, Object* b) : ctx(a), originalCtx(a), parent(b) {
	time.spin = time.move = 0.0;
}

Object::~Object() {
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
	if(ctx.altTexture != nullptr) {
		ctx.altTexture->initGL();
	}
	
	ctx.shader->Initialize();
	
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
	
	if(parent != nullptr) {
		//Move into place
		//Add the scales to the distance to make certain they never overlap
		modelMat= glm::rotate(parentModel, -time.move, glm::vec3(0.0, 1.0, 0.0));
		modelMat= glm::translate(modelMat, glm::vec3(pow(parent->ctx.scale, scaleExp) + pow(ctx.scale, scaleExp) + pow(ctx.orbitDistance, scaleExp), 0.0, 0.0));
		modelMat= glm::rotate(modelMat, time.move, glm::vec3(0.0, 1.0, 0.0));
	} else {
		modelMat = parentModel;
	}
	
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

void Object::Render(float lightPower) const {
	ctx.shader->Enable();

	//Send our shaders the MVP matrices
	ctx.shader->uniformMatrix4fv("modelMatrix", 1, GL_FALSE, glm::value_ptr(modelMat));
	ctx.shader->uniformMatrix4fv("viewMatrix", 1, GL_FALSE, glm::value_ptr(*viewMatrix));
	ctx.shader->uniformMatrix4fv("projectionMatrix", 1, GL_FALSE, glm::value_ptr(*projectionMatrix));
	
	//Send the material information
	ctx.shader->uniform3fv("MaterialAmbientColor", 1, &ctx.model->material.ambient.r);
	ctx.shader->uniform3fv("MaterialDiffuseColor", 1, &ctx.model->material.diffuse.r);
	ctx.shader->uniform3fv("MaterialSpecularColor", 1, &ctx.model->material.specular.r);
	
	//And light
	ctx.shader->uniform1fv("lightPower", 1, &lightPower);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	//Now send vertices, uvs, and normals
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
	
	//Send all our face information
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	
	//If we have a texture, use it
	if(ctx.texture != nullptr) {
		ctx.texture->bind(GL_COLOR_TEXTURE);
		ctx.shader->uniform1i("gSampler", GL_COLOR_TEXTURE_OFFSET);
	}
	if(ctx.altTexture != nullptr) {
		ctx.altTexture->bind(GL_ALT_TEXTURE);
		ctx.shader->uniform1i("gAltSampler", GL_ALT_TEXTURE_OFFSET);
	}
	
	//Now draw everything
	glDrawElements(GL_TRIANGLES, ctx.model->_indices.size(), GL_UNSIGNED_INT, 0);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Now pass the function down the chain to our satellites
	for (const auto &i : _children) {
		i->Render(lightPower);
	}
}

Object& Object::addChild(const Object::Context& ctx) {
	auto* newPlanet = new Object(ctx, this);
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
