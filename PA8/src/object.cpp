#include <Menu.h>
#include "object.h"

glm::mat4* Object::viewMatrix;
glm::mat4* Object::projectionMatrix;
Shader* Object::orbitShader;
// TODO: fix this global offset stuff
glm::vec3 globOff = {0,0,-2};
glm::vec3 const * Object::globalOffset = &globOff;
Menu* Object::menu;

Object::Object(const Context &a) : ctx(a), originalCtx(a), position(glm::vec3(ctx.xLoc, ctx.yLoc, ctx.zLoc)) {
	//Default value - just in case anything tries to read it
	//The planet doesn't actually start here - this will be updated in Update()
	_position = {a.xLoc, a.yLoc, a.zLoc};
	modelMat = glm::translate(modelMat, position);
}

Object::~Object() {}

void Object::Init_GL() {
	//Initialise shaders
	ctx.shader->Initialize();
	
	//Initialise models
	if(ctx.model != nullptr)
	{
		ctx.model->initGL();
	}

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
}

void Object::Update(float dt) {
	if(ctx.hasPhysics)
	{
		btTransform transformObject;
		ctx.physicsBody->getMotionState()->getWorldTransform(transformObject);

		//16 element matrix
		float mat[16];
		transformObject.getOpenGLMatrix(mat);
		modelMat = glm::make_mat4(mat);
	}


	// Get Model from physics
//	modelMat = glm::translate(glm::vec3(0) - *globalOffset);
}

const glm::mat4& Object::GetModel() const {
	return modelMat;
}

void Object::Render(float lightPower, GLuint shadowMap) const {
	
	//If we can't see this object, don't render it
//	Object* lookingAt = menu->getPlanet(menu->options.lookingAt);
//	if(lookingAt != this && menu->options.scale > 0.9) {
//		return;
//	}
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
//    if(ctx.isDynamic || ctx.isLightSource)
//    {
		float timeMod = 16 / 1000.0f;
        ctx.shader->uniform1fv("shaderTime", 1, &timeMod);
//    }
	
	//Enable blending (for transparent stuff)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Now draw our planet
	ctx.model->drawModel();

	glBindTexture(GL_TEXTURE_2D, 0);
	
	glDisable(GL_BLEND);
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
}