// This file actually contains all the logic behind the UI

#include "includes.h"

// PHYXEL ENGINE SETUP
#include "phyxel_config.h"

#define WINDOW_NAME "PhyxelEngine 1.0"
ImVec4 clear_color(0.45f, 0.55f, 0.60f, .00f);
float phyxel_size = 5; // actual pixels per phyxel
bool done = false;
char materialID = 0;
unsigned long long int iteration = 0;
bool play = false;

/// @brief Displays single button to select a material
/// @param thisMaterialId id of the material to be selected on click
/// @param label label of the button
/// @param color color of the button
void show_material_selection_button(char thisMaterialId, string label, ImU32 color) {
	const bool selected = materialID == thisMaterialId;
	const int button_size = 50;
	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImGui::PushStyleColor(ImGuiCol_Text, INV_COL32(color) | IM_COL32_A_MASK);
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
/// @param materials list of materials which are (label, color) pairs (the materialId is selected using their index in the list)
void show_material_selection_buttons(int ncols, vector<tuple<string, ImU32>> materials) {
	for (int i = 0; i < materials.size(); i++) {
		show_material_selection_button(
			i, get<0>(materials[i]), get<1>(materials[i])
		);
		if (i % ncols < ncols - 1)
			ImGui::SameLine();
	}
}

// THREE FUNCTIONS RESPONSIBLE FOR THE LOOK OF THE GUI

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

	// this is important. otherwise you're going to have a segfault right at the start
	phx::scene.fill(air);
	phx::scene.fillFrame(bedrock); // this is optional but suggested for now
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

	ImGui::Text("Materials selection");
	show_material_selection_buttons(4, {
		{"breath\nable\n air", IM_COL32(0, 0, 0, 0)},
		{"dry \nsand", IM_COL32(255, 225, 180, 255)},
		{"liquid\n water", IM_COL32(50, 150, 255, 255)},
		{"unkn\nown \nmetal", IM_COL32(170, 160, 170, 255)},
		{"toxic\n gas", IM_COL32(155, 255, 0, 60)},
		{"myste\nrious\n acid", IM_COL32(100, 255, 100, 255)},
		{"techn\nical \nbedrock", IM_COL32(0, 0, 0, 255)},
	});

	ImGui::End();

	// Displaying Simulation window
	stringstream ss;
	ss << "Simulation - Iteration: " << iteration;
	if (!play)
		ss << " Paused ";
	ss << "###simulation_window";
	ImGui::Begin(ss.str().c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	const ImVec2 window_size = {
		(float)(phyxel_size * PHX_SCENE_SIZE_X) + 15,
		(float)(phyxel_size * PHX_SCENE_SIZE_Y) + 30};
	ImGui::SetWindowSize(window_size);

	const auto p = ImGui::GetCursorScreenPos();
	const auto cursor = ImGui::GetCursorPos();
	auto available_space = ImGui::GetContentRegionAvail();
	if (abs(available_space.x) <= 1)
		available_space.x = 1;
	if (abs(available_space.y) <= 1)
		available_space.y = 1;
	// https://github.com/ocornut/imgui/issues/3149
	ImGui::InvisibleButton("canvas", available_space,
						   ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_AllowOverlap);
	ImGui::SetItemAllowOverlap();

	// Handling left click on the simulation field
	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		auto mousePos = ImGui::GetMousePos();
		phx::scene.setMaterial(
			(mousePos.x - p.x) / phyxel_size,
			(mousePos.y - p.y) / phyxel_size,
			phx::MaterialsList::get(materialID),
			rand());
	}

	// Rendering the simulation
	ImDrawList *draw_list = ImGui::GetWindowDrawList();

	// drawing vertical lines
	const auto lines_color = IM_COL32(255, 255, 255, 50);
	for (size_t i = 0; i < PHX_VIEW_SIZE_X; i++)
		draw_list->AddLine(
			{p.x + i * phyxel_size, p.y},
			{p.x + i * phyxel_size, p.y + window_size.y},
			lines_color);

	// drawing horizontal ones
	for (size_t i = 0; i < PHX_VIEW_SIZE_Y; i++)
		draw_list->AddLine(
			{p.x, p.y + i * phyxel_size},
			{p.x + window_size.x, p.y + i * phyxel_size},
			lines_color);

	// Drawing phyxels
	for (size_t i = 0; i < PHX_SCENE_SIZE_X * PHX_SCENE_SIZE_Y; i++)
	{
		int x = i % PHX_SCENE_SIZE_X;
		int y = i / PHX_SCENE_SIZE_X;
		phx::Color c = phx::scene.getColor(i);
		draw_list->AddRectFilled(
			{p.x + x * phyxel_size, p.y + y * phyxel_size},
			{p.x + (x + 1) * phyxel_size, p.y + (y + 1) * phyxel_size},
			IM_COL32(c.r, c.g, c.b, c.a));
	}

	ImGui::SetCursorPos(cursor);
	ImGui::End();
}