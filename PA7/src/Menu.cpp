#include "Menu.h"

Menu::Menu(Window& a, Object& b) : window(a), root(b), options(_options) {
	//Attach ImGUI to our SDL window
	ImGui_ImplSdlGL3_Init(window.getSDL_Window());
	
	//Defaults
	_options.numPlanets = 0;
	_options.planetSelector = 0;
	satelliteList = "";
	
	//Now build our satellite drop-down list
	//We're assuming this will never change after construction of the Menu object
	buildSatelliteList(root, "", _options.numPlanets);
}

void Menu::update(int dt, float width, float height) {

	ImGui_ImplSdlGL3_NewFrame(window.getSDL_Window());
    ImGuiIO& io = ImGui::GetIO();	
	//ImGui::ShowTestWindow();
	
	//Start a new window called "Controls"
	//On first seeing this window, make it 650 x 400, but let the user resize it later
	ImGui::SetNextWindowSize(ImVec2(550, 300), ImGuiCond_FirstUseEver);
    // ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing); 
	ImGui::Begin("Controls", NULL, ImGuiWindowFlags_ShowBorders);
	
	//Zoom slider
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * .55f);
	ImGui::SliderFloat("Camera Distance", &_options.zoom, 0.1f, 10.0f); ImGui::SameLine(0,10);
	if(ImGui::Button("Reset##zoom", ImVec2(40, 20))) _options.zoom = 1.0;
	
	ImGui::SliderFloat("Camera Rotation", &_options.rotation, 0.0f, 360.0f); ImGui::SameLine(0,10);
	if(ImGui::Button("Reset##rot", ImVec2(40, 20))) _options.rotation = 1.0;
	
	ImGui::RadioButton("Realistic Scale", &scaleTo, 1); ImGui::SameLine();
	ImGui::RadioButton("Close Scale", &scaleTo, 0);
	
	ImGui::Checkbox("Draw Orbits", &_options.drawOrbits); ImGui::SameLine();
	ImGui::Checkbox("Planet Labels", &_options.drawLabels);
	

    if (ImGui::CollapsingHeader("Planet Controls", ImGuiTreeNodeFlags_Framed | ImGuiCond_Appearing))
    {
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * .6f);
    	ImGui::BeginChild("Planet Controls", ImVec2(0, 145), true, ImGuiWindowFlags_NoScrollbar);

    	
    	//Get information of whatever planet we're currently looking at
    	Object::Context& ctx = getPlanet(_options.planetSelector)->ctx;
    	const Object::Context& originalCtx = getPlanet(_options.planetSelector)->originalCtx;
    	
        //Drop down planet list
        //Change the planet we're looking at
        if(ImGui::Combo("Select Satellite", &_options.planetSelector, satelliteList.c_str()))
        {
             _options.lookingAt = _options.planetSelector;
        };
        
    	
    	//Time Scale slider and reset (logarithmic slider [base 10])
    	ImGui::SliderFloat("Time Scale", &ctx.timeScale, -1000.0f, 1000.0, "%.3f", 10.0f); ImGui::SameLine(ImGui::GetWindowWidth() - 50);
    	if(ImGui::Button("Reset##timeScale")) ctx.timeScale = originalCtx.timeScale;
    	
    	//Orbit speed, reset, and direction
    	ImGui::SliderFloat("Orbit Speed", &ctx.moveScale, 0.0, 5.0); ImGui::SameLine(ImGui::GetWindowWidth() - 50);
    	if(ImGui::Button("Reset##moveScale")) ctx.moveScale = originalCtx.moveScale, ctx.moveDir = originalCtx.moveDir;
    	ImGui::RadioButton("cw##move", &ctx.moveDir, -1); ImGui::SameLine();
    	ImGui::RadioButton("ccw##move", &ctx.moveDir, 1);
    	
    	//Spin speed, reset, and direction
    	ImGui::SliderFloat("Spin Speed", &ctx.spinScale, 0.0, 5.0); ImGui::SameLine(ImGui::GetWindowWidth() - 50);
    	if(ImGui::Button("Reset##spinScale")) ctx.spinScale = originalCtx.spinScale, ctx.spinDir = originalCtx.spinDir;
    	ImGui::RadioButton("cw##spin", &ctx.spinDir, -1); ImGui::SameLine();
    	ImGui::RadioButton("ccw##spin", &ctx.spinDir, 1);
    	
    	ImGui::EndChild();
	}
	//End the window
	ImGui::End();
	
	//Planet Labels
	if(options.drawLabels) {
		//Make a new transparent window that covers the whole screen and doesn't take user input
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0, 0.0, 0.0, 0.0));
		
		ImGui::Begin("Overlay", NULL,
		             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs |
		             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
		
		//Loop through each planet and find where they are on the screen
		//Then draw a label there
		glm::vec4 screenPos;
		for (const auto& pair : satelliteMap) {
			//Multiply planet position by VP matrix
			//No Model Matrix because planet position is already in world coordinates
			screenPos = *Object::projectionMatrix *
			            (*Object::viewMatrix * (glm::vec4(pair.second->position - *Object::globalOffset, 1.0)));
			//If not on the screen
			if (screenPos.w < 0) continue;
			//Here we do height - because OpenGL's origin is at the bottom left of the screen and ImGui's is at the top left
			ImGui::SetCursorPos(ImVec2(((screenPos.x) / screenPos.w + 1) * width / 2 + 5,
			                           height - ((screenPos.y) / screenPos.w + 1) * height / 2 - 5));
			ImGui::Text(pair.second->ctx.name.c_str());
		}
		
		ImGui::End();
		ImGui::PopStyleColor(1);
	}
	
	updateScale(dt);
}

void Menu::updateScale(int dt) {
	if((_options.scale >= 1.0 && scaleTo == 1) || (_options.scale <= CLOSE_SCALE && scaleTo == 0)) {
		return;
	}
	
	//Take 2 seconds to scale
	float scaleBy = (1.0f - CLOSE_SCALE) * (dt / 1000.0f / 4.0f);
	
	if(scaleTo == 0) {
		scaleBy *= -1;
	}
	
	_options.scale += scaleBy;
	if(_options.scale < CLOSE_SCALE) _options.scale = CLOSE_SCALE;
	else if(_options.scale > 1.0) _options.scale = 1.0;
}

void Menu::render() {
	ImGui::Render();
}

void Menu::buildSatelliteList(Object& root, std::string pre, int& numSatellites) {
	//Build a tree-like format where it's clear what orbits what
	//-Sun
	//|-Mercury
	//|-Venus
	//|-Earth
	//||-The Moon
	//|-Mars
	satelliteList += pre;
	satelliteList += '-';
	satelliteList += root.ctx.name;
	satelliteList += '\0';
	
	satelliteMap[numSatellites] = &root;
	
	numSatellites++;
	
	//Now add all the satellites
	for(int i = 0; i < root.getNumChildren(); i++) {
		buildSatelliteList(root[i], pre + '|', numSatellites);
	}
}

Object* Menu::getPlanet(int index) const {
	return satelliteMap.at(index);
}

void Menu::setZoom(float zoom) {
	if(zoom < 0.1) zoom = 0.1;
	else if(zoom > 10.0) zoom = 10.0;
	
	_options.zoom = zoom;
}

void Menu::setRotation(float rotation){
	while(rotation < 0) rotation += 360.0f;
	while(rotation > 360) rotation -= 360.0f;
	
	_options.rotation = rotation;
}