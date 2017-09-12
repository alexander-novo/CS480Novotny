#include "Menu.h"

Menu::Menu(Window& a, Object& b) : window(a), root(b), options(_options) {
	ImGui_ImplSdlGL3_Init(window.getSDL_Window());
	
	_options.numPlanets = 0;
	_options.planetSelector = 0;
	satelliteList = "";
	buildSatelliteList(root, "", _options.numPlanets);
}

void Menu::update() {
	ImGui_ImplSdlGL3_NewFrame(window.getSDL_Window());
	
	//ImGui::ShowTestWindow();
	
	ImGui::Begin("Controls");
	
	Object::Context& ctx = getCurrentPlanet()->ctx;
	const Object::Context& originalCtx = getCurrentPlanet()->originalCtx;
	
	ImGui::Combo("Select Satellite", &_options.planetSelector, satelliteList.c_str());
	ImGui::SliderFloat("Time Scale", &ctx.timeScale, -5.0f, 5.0); ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if(ImGui::Button("Reset##timeScale")) ctx.timeScale = originalCtx.timeScale;
	
	ImGui::SliderFloat("Orbit Speed", &ctx.moveScale, 0.0, 5.0); ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if(ImGui::Button("Reset##moveScale")) ctx.moveScale = originalCtx.moveScale, ctx.moveDir = originalCtx.moveDir;
	ImGui::RadioButton("cw##move", &ctx.moveDir, -1); ImGui::SameLine();
	ImGui::RadioButton("ccw##move", &ctx.moveDir, 1);
	
	ImGui::SliderFloat("Spin Speed", &ctx.spinScale, 0.0, 5.0); ImGui::SameLine(ImGui::GetWindowWidth() - 50);
	if(ImGui::Button("Reset##spinScale")) ctx.spinScale = originalCtx.spinScale, ctx.spinDir = originalCtx.spinDir;
	ImGui::RadioButton("cw##spin", &ctx.spinDir, -1); ImGui::SameLine();
	ImGui::RadioButton("ccw##move", &ctx.spinDir, 1);
	
	ImGui::End();
}

void Menu::render() {
	ImGui::Render();
}

void Menu::buildSatelliteList(Object& root, std::string pre, int& numSatellites) {
	satelliteList += pre;
	satelliteList += '-';
	satelliteList += root.ctx.name;
	satelliteList += '\0';
	
	satelliteMap[numSatellites] = &root;
	
	numSatellites++;
	
	for(int i = 0; i < root.children.size(); i++) {
		buildSatelliteList(root.children[i], pre + '|', numSatellites);
	}
}

Object* Menu::getCurrentPlanet() {
	return satelliteMap[_options.planetSelector];
}