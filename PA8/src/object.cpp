#include <Menu.h>
#include "object.h"

glm::mat4* Object::viewMatrix;
glm::mat4* Object::projectionMatrix;
Shader* Object::orbitShader;
glm::vec3 const * Object::globalOffset;
Menu* Object::menu;

Object::Object(const Context &a, Object* b) : ctx(a), originalCtx(a), parent(b), position(_position) {
	//Make the planet start at a random position
	time.move = ((float) rand()) / RAND_MAX * 2 * M_PI;
	time.spin = 0;
	//Default value - just in case anything tries to read it
	//The planet doesn't actually start here - this will be updated in Update()
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

void Object::Update(float dt, float scaleExp) {
	double timeMod = dt / 1000.0f * ctx.timeScale;
	double scaleMult = ctx.scaleMultiplier / pow(ctx.scaleMultiplier, scaleExp); //Makes the sun always the same size
	double scale = scaleMult * pow(ctx.scale, scaleExp);
	
	//Update the timer
	time.spin += timeMod * ctx.spinScale * ctx.spinDir;
	time.move += timeMod * ctx.moveScale * ctx.moveDir;
	
	if(parent != nullptr) {
		//Move into place
		//Add the scales to the distance to make certain they never overlap
		double radius = scaleMult * pow(parent->ctx.scale, scaleExp) + scaleMult * pow(ctx.scale, scaleExp) + pow(ctx.orbitDistance, scaleExp);
		
		double cosTheta = cos(-time.move);
		_position = {parent->position.x + radius * cos(ctx.orbitTilt) * cosTheta,
		             parent->position.y + radius * sin(ctx.orbitTilt) * cosTheta,
		             parent->position.z + radius * sin(-time.move)};
		modelMat = glm::translate(position - *globalOffset);
	} else {
		modelMat = glm::translate(glm::vec3(0) - *globalOffset);
	}
	
	//Update all satellites after moving, so they follow us around
	for (auto &i : _children) {
		i->Update(dt * ctx.timeScale, scaleExp);
	}
	
	//Then rotate and scale so the satellites are unaffected
	modelMat= glm::rotate(modelMat, ctx.axisTilt + ctx.orbitTilt, glm::vec3(0.0, 0.0, 1.0));
	modelMat= glm::rotate(modelMat, -time.spin, glm::vec3(0.0, 1.0, 0.0));
	modelMat= glm::scale(modelMat, glm::vec3(scale, scale, scale));
	
}

const glm::mat4& Object::GetModel() const {
	return modelMat;
}

void Object::Render(float lightPower, GLuint shadowMap) const {
	
	//If we can't see this object, don't render it
	Object* lookingAt = menu->getPlanet(menu->options.lookingAt);
	if(lookingAt != this && menu->options.scale > 0.9) {
		for (const auto &i : _children) {
			i->Render(lightPower, shadowMap);
		}
		return;
	}
	ctx.shader->Enable();

	//Send our shaders the MVP matrices
	glm::mat4 modelViewMatrix = *viewMatrix * modelMat;
	ctx.shader->uniformMatrix4fv("modelMatrix", 1, GL_FALSE, glm::value_ptr(modelMat));
	ctx.shader->uniformMatrix4fv("viewMatrix", 1, GL_FALSE, glm::value_ptr(*viewMatrix));
	ctx.shader->uniformMatrix4fv("projectionMatrix", 1, GL_FALSE, glm::value_ptr(*projectionMatrix));
	ctx.shader->uniformMatrix4fv("modelViewMatrix", 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
	
	//Send the material information
	ctx.shader->uniform3fv("MaterialAmbientColor", 1, &ctx.model->material.ambient.r);
	ctx.shader->uniform3fv("MaterialDiffuseColor", 1, &ctx.model->material.diffuse.r);
	ctx.shader->uniform3fv("MaterialSpecularColor", 1, &ctx.model->material.specular.r);
	ctx.shader->uniform1fv("shininess", 1, &ctx.model->material.shininess);
	
	glm::vec3 oppOffset = *Object::globalOffset;
	oppOffset *= -1;
	
	ctx.shader->uniform3fv("lightW3", 1, glm::value_ptr(oppOffset));
	
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
	if(menu->options.drawShadows) {
		glActiveTexture(GL_SHADOW_TEXTURE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
		ctx.shader->uniform1i("gShadowMap", GL_SHADOW_TEXTURE_OFFSET);
		ctx.shader->uniform1i("usingShadows", 1);
	} else {
		ctx.shader->uniform1i("usingShadows", 0);
	}
	
	//Timer for shader
	ctx.shader->uniform1fv("shaderTime", 1, &time.spin);
	
	//Enable blending (for transparent stuff)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Now draw our planet
	ctx.model->drawModel();

	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisable(GL_BLEND);


	//Now pass the function down the chain to our satellites
	for (const auto &i : _children) {
		i->Render(lightPower, shadowMap);
	}

}

void Object::renderShadow(Shader* shadowShader) const {
	
	//If we can't see this object, don't render it
	Object* lookingAt = menu->getPlanet(menu->options.lookingAt);
	if(lookingAt != this && menu->options.scale > 0.9) {
		return;
	}
	
	shadowShader->uniformMatrix4fv("M", 1, GL_FALSE, glm::value_ptr(modelMat));
	
	//Now draw our planet
	ctx.model->drawModel();
	
	//Now pass the function down the chain to our satellites
	for (const auto &i : _children) {
		i->renderShadow(shadowShader);
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