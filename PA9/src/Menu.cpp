#include "Menu.h"

Menu::Menu(Window& a) : window(a), options(_options) {
	//Attach ImGUI to our SDL window
	ImGui_ImplSdlGL3_Init(window.getSDL_Window());
}

void Menu::update(int dt, float width, float height) {
	
	ImGui_ImplSdlGL3_NewFrame(window.getSDL_Window());
	ImGuiIO& io = ImGui::GetIO();
}

void Menu::render() {
	ImGui::Render();
}

void Menu::setZoom(float zoom) {
	if (zoom < 0.1) zoom = 0.1;
	else if (zoom > 50.0) zoom = 50.0;
	
	_options.zoom = zoom;
}

void Menu::setRotation(float rotation) {
	while (rotation < 0) rotation += 360.0f;
	while (rotation > 360) rotation -= 360.0f;
	
	_options.rotation = rotation;
}