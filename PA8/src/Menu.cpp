#include "Menu.h"

Menu::Menu(Window& a) : window(a), options(_options) {
	//Attach ImGUI to our SDL window
	ImGui_ImplSdlGL3_Init(window.getSDL_Window());
	
	//Defaults
	_options.numPlanets = 0;
	_options.planetSelector = 0;
	satelliteList = "";
	
	//Now build our satellite drop-down list
	//We're assuming this will never change after construction of the Menu object
}

void Menu::update(int dt, float width, float height) {
	
	ImGui_ImplSdlGL3_NewFrame(window.getSDL_Window());
	ImGuiIO& io = ImGui::GetIO();

	//Planet Labels
	if (options.drawLabels || options.drawMoonLabels) {
		//Make a new transparent window that covers the whole screen and doesn't take user input
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0, 0.0, 0.0, 0.0));
		
		ImGui::Begin("Overlay", NULL,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs |
		             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
		             ImGuiWindowFlags_NoFocusOnAppearing);
		
		//Loop through each planet and find where they are on the screen
		//Then draw a label there
		glm::vec4 screenPos;
		for (const auto& pair : satelliteMap) {
			if ((!options.drawLabels))
				continue;
			//Multiply planet position by VP matrix
			//No Model Matrix because planet position is already in world coordinates
			screenPos = *Object::projectionMatrix *
			            (*Object::viewMatrix * (glm::vec4(pair.second->position - *Object::globalOffset, 1.0)));
			//If not on the screen
			if (screenPos.w < 0) continue;
			//Here we do height - because OpenGL's origin is at the bottom left of the screen and ImGui's is at the top left
			ImGui::SetCursorPos(ImVec2(((screenPos.x) / screenPos.w + 1) * width / 2 + 5,
			                           height - ((screenPos.y) / screenPos.w + 1) * height / 2 - 5));
			
			ImGui::Text("%s", pair.second->ctx.name.c_str());
		}
		
		ImGui::End();
		ImGui::PopStyleColor(1);
	}
	
	updateScale(dt);
}

void Menu::updateScale(int dt) {
	if ((_options.scale >= 1.0 && scaleTo == 1) || (_options.scale <= CLOSE_SCALE && scaleTo == 0)) {
		return;
	}
	
	//Take 2 seconds to scale
	float scaleBy = (1.0f - CLOSE_SCALE) * (dt / 1000.0f / 4.0f);
	
	if (scaleTo == 0) {
		scaleBy *= -1;
	}
	
	_options.scale += scaleBy;
	if (_options.scale < CLOSE_SCALE) _options.scale = CLOSE_SCALE;
	else if (_options.scale > 1.0) _options.scale = 1.0;
}

void Menu::render() {
	ImGui::Render();
}


Object* Menu::getPlanet(int index) const {
	return satelliteMap.at(index);
}

void Menu::setZoom(float zoom) {
	if (zoom < 0.1) zoom = 0.1;
	else if (zoom > 10.0) zoom = 10.0;
	
	_options.zoom = zoom;
}

void Menu::setRotation(float rotation) {
	while (rotation < 0) rotation += 360.0f;
	while (rotation > 360) rotation -= 360.0f;
	
	_options.rotation = rotation;
}