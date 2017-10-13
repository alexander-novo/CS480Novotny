#include "object.h"

glm::mat4* Object::viewMatrix;
glm::mat4* Object::projectionMatrix;
Shader* Object::orbitShader;

Object::Object(const Context &a, Object* b) : ctx(a), originalCtx(a), parent(b), position(_position) {
	time.spin = 0;
	//Prevent Saturn from Starting away from its rings
	if(a.name == "Saturn" || a.name == "Saturn Rings" )
	{
		time.move = 0;
	}
	else
	{
		time.move = ((float) rand()) / RAND_MAX * 2 * M_PI;	
	}
	_position = {0.0, 0.0, 0.0};
}

Object::~Object() {
	for (auto &i : _children) {
		delete i;
	}
	_children.clear();
}

void Object::Init_GL() {
	//Initialise shaders
	orbitShader->Initialize();
	ctx.shader->Initialize();
	
	//Initialise models
	ctx.model->initGL();
	
	//Set aside a buffer for our orbit vertices
	glGenBuffers(1, &OB);
	
	//Initialise textures
	if(ctx.texture != nullptr) {
		ctx.texture->initGL();
	}
	if(ctx.altTexture != nullptr) {
		ctx.altTexture->initGL();
	}
	if(ctx.normalMap != nullptr) {
		ctx.normalMap->initGL();
	}
	if(ctx.specularMap != nullptr) {
		ctx.specularMap->initGL();
	}
	
	//Now do the same for all our children
	for(auto& child : _children) {
		child->Init_GL();
	}
}

void Object::Update(float dt, float scaleExp, bool drawOrbits) {
	float timeMod = dt / 1000.0f * ctx.timeScale;
	float scaleMult = ctx.scaleMultiplier / pow(ctx.scaleMultiplier, scaleExp);
	float scale = scaleMult * pow(ctx.scale, scaleExp);
	
	//Update the timer
	time.spin += timeMod * ctx.spinScale * ctx.spinDir;
	time.move += timeMod * ctx.moveScale * ctx.moveDir;
	
	if(parent != nullptr) {
		//Move into place
		//Add the scales to the distance to make certain they never overlap
		float radius = scaleMult * pow(parent->ctx.scale, scaleExp) + scaleMult * pow(ctx.scale, scaleExp) + pow(ctx.orbitDistance, scaleExp);
		
		float cosTheta = cos(-time.move);
		_position = {parent->position.x + radius * cos(ctx.orbitTilt) * cosTheta,
		             parent->position.y + radius * sin(ctx.orbitTilt) * cosTheta,
		             parent->position.z + radius * sin(-time.move)};
		modelMat = glm::translate(position);
	} else {
		modelMat = glm::mat4(1.0);
	}
	
	//Update all satellites after moving, so they follow us around
	for (auto &i : _children) {
		i->Update(dt * ctx.timeScale, scaleExp, drawOrbits);
	}
	
	//Then rotate and scale so the satellites are unaffected
	modelMat= glm::rotate(modelMat, ctx.axisTilt, glm::vec3(0.0, 0.0, 1.0));
	modelMat= glm::rotate(modelMat, -time.spin, glm::vec3(0.0, 1.0, 0.0));
	modelMat= glm::scale(modelMat, glm::vec3(scale, scale, scale));
	
	if(drawOrbits) updateOrbit(scaleExp, scaleMult);
}

void Object::updateOrbit(float scaleExp, float scaleMult) {
	if(parent == nullptr) return;
	
	const glm::vec3& parentPosition = parent->position;
	
	if((parentPosition == orbitInfo.lastParentPos && scaleExp == orbitInfo.lastScale)) return;
	
	orbitInfo.lastParentPos = parentPosition;
	orbitInfo.lastScale = scaleExp;
	
	float radius = scaleMult * pow(parent->ctx.scale, scaleExp) + scaleMult * pow(ctx.scale, scaleExp) + pow(ctx.orbitDistance, scaleExp);
	double thetaStep = M_PI / 300;
	double rCosPhi = radius * cos(ctx.orbitTilt);
	double rSinPhi = radius * sin(ctx.orbitTilt);
	double theta = 0;
	
	orbitVertices.clear();
	
	for(int i = 0; i * thetaStep < M_PI * 2; i++){
		theta = i * thetaStep;
		orbitVertices.push_back(glm::vec3(parentPosition.x + rCosPhi * cos(theta),
		                                  parentPosition.y + rSinPhi * cos(theta),
		                                  parentPosition.z + radius * sin(theta)));
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, OB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * orbitVertices.size(), &orbitVertices[0], GL_STATIC_DRAW);
}

const glm::mat4& Object::GetModel() const {
	return modelMat;
}

void Object::Render(float lightPower, bool drawOrbits) const {
	if(drawOrbits) drawOrbit();
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
	
	//If we have a texture, use it
	if(ctx.texture != nullptr) {
		ctx.texture->bind(GL_COLOR_TEXTURE);
		ctx.shader->uniform1i("gSampler", GL_COLOR_TEXTURE_OFFSET);
	}
	if(ctx.altTexture != nullptr) {
		ctx.altTexture->bind(GL_ALT_TEXTURE);
		ctx.shader->uniform1i("gAltSampler", GL_ALT_TEXTURE_OFFSET);
	}
	if(ctx.normalMap != nullptr) {
		ctx.normalMap->bind(GL_NORMAL_TEXTURE);
		ctx.shader->uniform1i("gNormalSampler", GL_NORMAL_TEXTURE_OFFSET);
	}
	if(ctx.specularMap != nullptr) {
		ctx.specularMap->bind(GL_SPECULAR_TEXTURE);
		ctx.shader->uniform1i("gSpecularSampler", GL_SPECULAR_TEXTURE_OFFSET);
	}
	
	//Timer for shader
	ctx.shader->uniform1fv("shaderTime", 1, &time.spin);
	
	//Now draw our planet
	ctx.model->drawModel();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//Now pass the function down the chain to our satellites
	for (const auto &i : _children) {
		i->Render(lightPower, drawOrbits);
	}
}

void Object::drawOrbit() const {
	if(parent == nullptr) return;
	orbitShader->Enable();
	
	orbitShader->uniformMatrix4fv("viewMatrix", 1, GL_FALSE, glm::value_ptr(*viewMatrix));
	orbitShader->uniformMatrix4fv("projectionMatrix", 1, GL_FALSE, glm::value_ptr(*projectionMatrix));
	
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, OB);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	
	glDrawArrays(GL_LINES, 0, orbitVertices.size());
	
	glDisableVertexAttribArray(0);
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
