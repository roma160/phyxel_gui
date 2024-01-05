// This file actually contains all the logic behind the UI

#include "includes.h"
#include "static_variables.hpp"
#include "phyxel_window.hpp"

/// @brief Displays single button to select a material
/// @param thisMaterialId id of the material to be selected on click
/// @param label label of the button
/// @param color color of the button
void show_material_selection_button(char thisMaterialId, string label, ImU32 color) {
	const bool selected = materialID == thisMaterialId;
	const int button_size = 50;
	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImU32 label_color;
	if (color >> IM_COL32_A_SHIFT < 5)
		label_color = IM_COL32_WHITE;
	else label_color = INV_COL32(color) | IM_COL32_A_MASK;
	ImGui::PushStyleColor(ImGuiCol_Text, label_color);
	if (selected)
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 6.0f);
	if (ImGui::Button(label.c_str(), {button_size, button_size})) {
		if (selected)
			ImGui::PopStyleVar();
		
		materialID = thisMaterialId;
	}
	else if (selected) {
		ImGui::PopStyleVar();
	}
	ImGui::PopStyleColor(2);
}

/// @brief Generate a table of buttons for material selection
/// @param ncols number of columns in the table
string wrap_label(const string& label) {
	static map<string, string> precomputed;
	auto buff = precomputed.find(label);
	if (buff != precomputed.end()) 
		return buff->second;
	
	string res = label;
	for (int i = 5; i < res.size(); i += 6)
		res.insert(i, 1, '\n');
	precomputed[label] = res;
	return res;
}
void show_material_selection_buttons(int ncols) {
	for (int i = 0; i < phx::MaterialsList::getSize(); i++) {
		const auto& material = phx::MaterialsList::get(i);
		const auto c = material->getColor(0);
		show_material_selection_button(
			i, wrap_label(material->name),
			IM_COL32(c.r, c.g, c.b, c.a)
		);
		if (i % ncols < ncols - 1)
			ImGui::SameLine();
	}
	ImGui::NewLine();
}

set<ImVec2ih> enumerate_brush_phyxels() {
	set<ImVec2ih> ret;
	const int delta = (brush_size - 1) / 2;
	for (int x = 0; x < brush_size; x++) {
		if (brush_pos.x + x - delta < 0 || brush_pos.x + x - delta >= PHX_SCENE_SIZE_X)
			continue;
		for (int y = 0; y < brush_size; y++) {
			if (brush_pos.y + y - delta < 0 || brush_pos.y + y - delta >= PHX_SCENE_SIZE_Y)
				continue;
			ret.insert({
				(short) (brush_pos.x + x - delta),
				(short) (brush_pos.y + y - delta)
			});
		}
	}
	return ret;
}

vector<PhyxelWindow> phyxel_windows = {
	PhyxelWindow(
		"Physical View", "physical_view",
		[](){
			PhyxelWindowRenderData ret;
			stringstream ss;
			ss << "Simulation - Iteration: " << iteration;
			if (!play)
				ss << " Paused ";
			ret.title = ss.str();

			constexpr ImVec4 selection_color(255, 255, 255, 125);
			const auto brush_phyxels = enumerate_brush_phyxels();
			ImVec4 buff;
			int x, y;

			for (int i = 0; i < PHX_SCENE_SIZE_X * PHX_SCENE_SIZE_Y; i++)
			{
				x = i % PHX_SCENE_SIZE_X;
				y = i / PHX_SCENE_SIZE_X;
				auto& scene_color = phx::scene.getColor(i);
				buff = { 
					(float) scene_color.r,
					(float) scene_color.g,
					(float) scene_color.b,
					(float) scene_color.a
				};
				// Highlighting a phyxel if it is under the brush
				if (brush_phyxels.find({(short) x, (short) y}) != brush_phyxels.end())
					buff = buff * .5 + selection_color * .5;
				
				ret.phyxels_colors[x][y] = IM_COL32(buff.x, buff.y, buff.z, buff.w);
			}
			return ret;
		},
		[](float x, float y){
			brush_pos = { (short) (x / phyxel_size), (short) (y / phyxel_size) };
			const auto brush_phyxels = enumerate_brush_phyxels();
			for (auto iter = brush_phyxels.begin(); iter != brush_phyxels.end(); iter++)
				phx::scene.setMaterial(
					iter->x, iter->y,
					phx::MaterialsList::get(materialID),
					rand()
				);
		},
		[](float x, float y) { brush_pos = { (short) (x / phyxel_size), (short) (y / phyxel_size) }; }
	),

	PhyxelWindow(
		"Temperature View", "temperature_view",
		[]() {
			float max = phx::scene.getTemperature(0),
			 	min = phx::scene.getTemperature(0),
				buff;
			for (size_t i = 0; i < PHX_SCENE_SIZE_X * PHX_SCENE_SIZE_Y; i++)
			{
				buff = phx::scene.getTemperature(i);
				if (buff > max) max = buff;
				else if (buff < min) min = buff;
			}

			PhyxelWindowRenderData ret;
			stringstream ss;
			ss << "Temperature at iteration : " << iteration << "; Max: " << max << ", Min: " << min;
			if (!play)
				ss << " Paused ";
			ret.title = ss.str();

			constexpr ImVec4 minCol(0, 0, 156, 255), maxCol(217, 0, 43, 255);
			for (size_t i = 0; i < PHX_SCENE_SIZE_X * PHX_SCENE_SIZE_Y; i++)
			{
				buff = (phx::scene.getTemperature(i) - min) / (max - min);
				auto color = (maxCol * buff) + (minCol * (1 - buff));
				ret.phyxels_colors[i % PHX_SCENE_SIZE_X][i / PHX_SCENE_SIZE_X] = 
					IM_COL32(
						(unsigned char) color.x,
						(unsigned char) color.y,
						(unsigned char) color.z,
						(unsigned char) color.w
					);
			}
			return ret;
		},
		[](float x, float y){

		}
	)
};

// THREE FUNCTIONS TO BE CALLED IN THE MAIN.CPP

void simulation_init()
{
	auto air = phx::MaterialsList::addMaterial("breathable air", 1, PHX_MTYPE_GAS, phx::Color(0, 0, 0, 0));
	air->isRemovable = true;
	auto sand = phx::MaterialsList::addMaterial("dry sand", 4, PHX_MTYPE_POD, phx::Color(255, 225, 180));
	sand->addColor(phx::Color(245, 245, 170));
	sand->addColor(phx::Color(190, 175, 170));
	auto water = phx::MaterialsList::addMaterial("liquid water", 3, PHX_MTYPE_LIQ, phx::Color(50, 150, 255));
	water->viscosity = 3000;
	auto metal = phx::MaterialsList::addMaterial("unknown metal", 5, PHX_MTYPE_SOL, phx::Color(170, 160, 170));
	auto toxic_gas = phx::MaterialsList::addMaterial("toxic gas", 0.4, PHX_MTYPE_GAS, phx::Color(155, 255, 0, 60));
	auto acid = phx::MaterialsList::addMaterial("mysterious acid", 3, PHX_MTYPE_LIQ, phx::Color(100, 255, 100));
	acid->addReaction(metal, air, toxic_gas);
	auto bedrock = phx::MaterialsList::addMaterial("technical bedrock", 0, PHX_MTYPE_SOL, phx::Color(0, 0, 0, 255)); // as for v1.0, a bedrock solid frame around the scene is suggested for multiple reasons. this is planned to be changed later

	auto wood = phx::MaterialsList::addMaterial("wood", 2, PHX_MTYPE_SOL, {217, 134, 0}, .25, true);
	auto carbonic_gas = phx::MaterialsList::addMaterial("carbonic gas", 0.3, PHX_MTYPE_GAS, {255, 255, 255, 0});
	auto ash = phx::MaterialsList::addMaterial("ash", 3.5, PHX_MTYPE_POD, {158, 158, 158});
	wood->setBurningParams(true, 1, ash, carbonic_gas, 200);
	acid->addReaction(wood, air, toxic_gas, 100, 300);

	// this is important. otherwise you're going to have a segfault right at the start
	phx::scene.fill(air);
	phx::scene.fillFrame(bedrock); // this is optional but suggested for now
}

void gui_init() {
	// Temperature View is not displayed at start
	phyxel_windows[1].show = false;
}

void simulation_step()
{
	if (play)
	{
		phx::scene.updateAll();
		iteration++;
	}
}

void gui_step()
{
	// Displaying the DemoWindow
	// https://stackoverflow.com/questions/2249282/c-c-portable-way-to-detect-debug-release
#ifdef _DEBUG
	ImGui::ShowDemoWindow();
#endif

	// Displaying the Controls window
	ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::Button("Toggle simulation"))
		play = !play;

	ImGui::DragFloat("Phyxel size", &phyxel_size, .01, 1, 100);
	ImGui::DragInt("Brush size", &brush_size, 1, 1, 50);

	ImGui::SeparatorText("Materials selection");
	show_material_selection_buttons(4);

	ImGui::SeparatorText("Windows toggles");
	for (int i = 0; i < phyxel_windows.size(); i++) {
		auto& w = phyxel_windows[i];
		ImGui::Text(w.name.c_str());
		ImGui::PushID(i);
		ImGui::Checkbox("Show", &w.show);
		ImGui::SameLine();
		ImGui::Checkbox("Grid", &w.use_grid);
		ImGui::PopID();
	}

	ImGui::End();

	// Displaying Simulation windows
	for (int i = 0; i < phyxel_windows.size(); i++) {
		phyxel_windows[i].render();
	}
}