#include "Menu.h"

Menu::Menu(Window& a) : window(a), options(_options) {
	_options.paused = false;
	//Attach ImGUI to our SDL window
	ImGui_ImplSdlGL3_Init(window.getSDL_Window());
}

void Menu::update(int dt, float width, float height) {
	
	ImGui_ImplSdlGL3_NewFrame(window.getSDL_Window());
	ImGuiIO& io = ImGui::GetIO();
	
	//ImGui::ShowTestWindow();
	
	_options.changedShadowSize = false;
	_options.shouldStartNewGame = false;
	int tempShadowSize = _options.shadowSize;
	int tempBallMode = _options.singleBall;
	
	if(ImGui::BeginMainMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::MenuItem("Options", "o")) _options.showOptionsMenu = true;
			ImGui::Separator();
			if(ImGui::MenuItem("Quit", "esc"))  _options.shouldClose = true;
			ImGui::EndMenu();
		}
		
		ImGui::EndMainMenuBar();
	}

	if(_options.isPlayingMusic && !window.isPlayingMusic)
	{
		window.PlayMusic(_options.isPlayingMusic);
	}
	else if(!_options.isPlayingMusic && window.isPlayingMusic)
	{
		window.PlayMusic(_options.isPlayingMusic);
	}


	if(_options.showPlayers) {

		ImGui::SetNextWindowSize(ImVec2(200,300));
		if (ImGui::Begin("Pool Table", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
            if (_options.playerCheckBoxDisabled) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); //PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            if (PhysicsWorld::game->isPlayer1) {
                _options.isPlayer1Turn = true;
                _options.isPlayer2Turn = false;
            } else {
                _options.isPlayer1Turn = false;
                _options.isPlayer2Turn = true;
            }
			if (PhysicsWorld::game->isNextShotOK)
			{
				_options.isShotReady = true;
			} else
			{
				_options.isShotReady = false;
			}
			ImGui::Text("Player Turn:");
            ImGui::Checkbox("Player 1", &_options.isPlayer1Turn);
            ImGui::Checkbox("Player 2", &_options.isPlayer2Turn);


            if (PhysicsWorld::game->isPlayer1Stripes) {
                _options.isPlayer1Stripes = true;
            }
            if (PhysicsWorld::game->isPlayer1Solids) {
                _options.isPlayer1Solids = true;
            }

            ImGui::Text("Player 1 is:");
            ImGui::Checkbox("Solids", &_options.isPlayer1Solids);
            ImGui::Checkbox("Stripes", &_options.isPlayer1Stripes);

            ImGui::Text("Ready for shot:");
            ImGui::SameLine();
			ImGui::Checkbox("", &_options.isShotReady);


            if (_options.playerCheckBoxDisabled) {
				ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
			if(ImGui::Button("New Game", ImVec2(100,30)))
			{
				isNewGame = true;
			}

            ImGui::End();
        }

	}

	if(_options.showOptionsMenu) {
		ImGui::SetNextWindowSize(ImVec2(600,680), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Options", &_options.showOptionsMenu, ImGuiWindowFlags_NoCollapse)) {
			
			if(ImGui::CollapsingHeader("Game Options")) {

				ImGui::Text("Music");
				ImGui::Indent(MENU_OPTIONS_INDENT);
				ImGui::Checkbox("Music", &_options.isPlayingMusic);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Enable/Disable music");
				ImGui::SameLine();
				ImGui::Checkbox("Sounds", &_options.isPlayingSounds);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Enable/Disable sounds");

				ImGui::Unindent(MENU_OPTIONS_INDENT);
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
	
	if(_options.singleBall != tempBallMode) {
		_options.shouldStartNewGame = true;
	}
}

void Menu::render() {
	ImGui::Render();
}

void Menu::setZoom(float zoom) {
	if (zoom < 0.1) zoom = 0.1;
	else if (zoom > 200.0) zoom = 200.0;
	
	_options.zoom = zoom;
}

void Menu::setRotation(float rotation) {
	while (rotation < 0) rotation += 360.0f;
	while (rotation > 360) rotation -= 360.0f;
	
	_options.rotation = rotation;
}

void Menu::setElevation(float elevation) {
	_options.elevation = elevation;
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

int Menu::singleBall(int singleBall) {
	static int single = 1;
	if(singleBall >= 0) {
		single = singleBall;
	}
	return single;
}

void Menu::pause() {
	_options.paused = !_options.paused;
}