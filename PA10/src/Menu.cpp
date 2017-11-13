#include "Menu.h"

Menu::Menu(Window& a) : window(a), options(_options) {
	//Attach ImGUI to our SDL window
	ImGui_ImplSdlGL3_Init(window.getSDL_Window());
}

void Menu::update(int dt, float width, float height) {
	
	ImGui_ImplSdlGL3_NewFrame(window.getSDL_Window());
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::ShowTestWindow();
	
	_options.changedShadowSize = false;
	int tempShadowSize = _options.shadowSize;
	
	if(ImGui::BeginMainMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("Options", "o")) _options.showOptionsMenu = true;
			ImGui::Separator();
			if(ImGui::MenuItem("Quit", "esc"))  _options.shouldClose = true;
			ImGui::EndMenu();
		}
		
		ImGui::EndMainMenuBar();
	}
	if(_options.showOptionsMenu) {
		ImGui::SetNextWindowSize(ImVec2(600,680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Options", &_options.showOptionsMenu, ImGuiWindowFlags_NoCollapse)) {
			
			if(ImGui::CollapsingHeader("Game Options")) {
				ImGui::ColorPicker3("Ambient Lighting", &_options.ambientColor.r, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_PickerHueBar);
			}
			
			if(ImGui::CollapsingHeader("Graphics Options")) {
				ImGui::Text("Shader Type");
				ImGui::Indent(MENU_OPTIONS_INDENT);
				ImGui::RadioButton("Vertex Lighting", &_options.shaderType, MENU_SHADER_VERTEX); ImGui::SameLine();
				ImGui::RadioButton("Fragment Lighting", &_options.shaderType, MENU_SHADER_FRAGMENT);
				
				ImGui::Unindent(MENU_OPTIONS_INDENT);
				ImGui::Text("Shadows");
				ImGui::Indent(MENU_OPTIONS_INDENT);
				ImGui::RadioButton("Off", &_options.shadowSize, MENU_SHADOWS_NONE); ImGui::SameLine();
				ImGui::RadioButton("Low", &_options.shadowSize, MENU_SHADOWS_LOW); ImGui::SameLine();
				ImGui::RadioButton("Medium", &_options.shadowSize, MENU_SHADOWS_MED); ImGui::SameLine();
				ImGui::RadioButton("High", &_options.shadowSize, MENU_SHADOWS_HIGH);
				
				if(tempShadowSize != _options.shadowSize) _options.changedShadowSize = true;
			}
			
			ImGui::End();
		}
		
		//Pause / unpause, depending on whether we closed the window this frame
		_options.paused = _options.showOptionsMenu;
	}
	if(_options.shaderType != prevShaderType) {
		prevShaderType = _options.shaderType;
		_options.shouldSwapShaders = true;
	} else {
		_options.shouldSwapShaders = false;
	}
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

void Menu::toggleOptionsMenu() {
	_options.showOptionsMenu = !_options.showOptionsMenu;
	if(!_options.showOptionsMenu) {
		_options.paused = false;
	}
}

void Menu::swapShaderType() {
	if(_options.shaderType == MENU_SHADER_VERTEX) {
		_options.shaderType = MENU_SHADER_FRAGMENT;
	} else {
		_options.shaderType = MENU_SHADER_VERTEX;
	}
}