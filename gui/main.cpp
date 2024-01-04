// Initial source is from here
// https://github.com/roma160/OOP_LAB2_sem2/blob/website/emscripten/main.cpp

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <tuple>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
// #include "../libs/emscripten/emscripten_mainloop_stub.h"
#include <emscripten.h>
#include <functional>
static std::function<void()>            MainLoopForEmscriptenP;
static void MainLoopForEmscripten()     { MainLoopForEmscriptenP(); }
#define EMSCRIPTEN_MAINLOOP_BEGIN       MainLoopForEmscriptenP = [&]()
#define EMSCRIPTEN_MAINLOOP_END         ; emscripten_set_main_loop(MainLoopForEmscripten, 0, true)
#else
#define EMSCRIPTEN_MAINLOOP_BEGIN
#define EMSCRIPTEN_MAINLOOP_END
#endif

#ifdef EMSCRIPTEN_CODE
#define DELTA_TICKS 10
#else
#define DELTA_TICKS 29
#endif

#define INV_COL32(COL32) (~IM_COL32_A_MASK) ^ COL32

// PHYXEL ENGINE SETUP
#include "phyxel_config.h"
#include "phyxel.hpp"
#define WINDOW_NAME "PhyxelEngine 1.0"
float phyxel_size = 5; // pixels per phyxel

// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_sdl2.cpp#L158
static ImGuiKey ImGui_ImplSDL2_KeycodeToImGuiKey(int keycode)
{
	switch (keycode)
	{
		case SDLK_TAB: return ImGuiKey_Tab;
		case SDLK_LEFT: return ImGuiKey_LeftArrow;
		case SDLK_RIGHT: return ImGuiKey_RightArrow;
		case SDLK_UP: return ImGuiKey_UpArrow;
		case SDLK_DOWN: return ImGuiKey_DownArrow;
		case SDLK_PAGEUP: return ImGuiKey_PageUp;
		case SDLK_PAGEDOWN: return ImGuiKey_PageDown;
		case SDLK_HOME: return ImGuiKey_Home;
		case SDLK_END: return ImGuiKey_End;
		case SDLK_INSERT: return ImGuiKey_Insert;
		case SDLK_DELETE: return ImGuiKey_Delete;
		case SDLK_BACKSPACE: return ImGuiKey_Backspace;
		case SDLK_SPACE: return ImGuiKey_Space;
		case SDLK_RETURN: return ImGuiKey_Enter;
		case SDLK_ESCAPE: return ImGuiKey_Escape;
		case SDLK_QUOTE: return ImGuiKey_Apostrophe;
		case SDLK_COMMA: return ImGuiKey_Comma;
		case SDLK_MINUS: return ImGuiKey_Minus;
		case SDLK_PERIOD: return ImGuiKey_Period;
		case SDLK_SLASH: return ImGuiKey_Slash;
		case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
		case SDLK_EQUALS: return ImGuiKey_Equal;
		case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
		case SDLK_BACKSLASH: return ImGuiKey_Backslash;
		case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
		case SDLK_BACKQUOTE: return ImGuiKey_GraveAccent;
		case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
		case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
		case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
		case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
		case SDLK_PAUSE: return ImGuiKey_Pause;
		case SDLK_KP_0: return ImGuiKey_Keypad0;
		case SDLK_KP_1: return ImGuiKey_Keypad1;
		case SDLK_KP_2: return ImGuiKey_Keypad2;
		case SDLK_KP_3: return ImGuiKey_Keypad3;
		case SDLK_KP_4: return ImGuiKey_Keypad4;
		case SDLK_KP_5: return ImGuiKey_Keypad5;
		case SDLK_KP_6: return ImGuiKey_Keypad6;
		case SDLK_KP_7: return ImGuiKey_Keypad7;
		case SDLK_KP_8: return ImGuiKey_Keypad8;
		case SDLK_KP_9: return ImGuiKey_Keypad9;
		case SDLK_KP_PERIOD: return ImGuiKey_KeypadDecimal;
		case SDLK_KP_DIVIDE: return ImGuiKey_KeypadDivide;
		case SDLK_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case SDLK_KP_MINUS: return ImGuiKey_KeypadSubtract;
		case SDLK_KP_PLUS: return ImGuiKey_KeypadAdd;
		case SDLK_KP_ENTER: return ImGuiKey_KeypadEnter;
		case SDLK_KP_EQUALS: return ImGuiKey_KeypadEqual;
		case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
		case SDLK_LSHIFT: return ImGuiKey_LeftShift;
		case SDLK_LALT: return ImGuiKey_LeftAlt;
		case SDLK_LGUI: return ImGuiKey_LeftSuper;
		case SDLK_RCTRL: return ImGuiKey_RightCtrl;
		case SDLK_RSHIFT: return ImGuiKey_RightShift;
		case SDLK_RALT: return ImGuiKey_RightAlt;
		case SDLK_RGUI: return ImGuiKey_RightSuper;
		case SDLK_APPLICATION: return ImGuiKey_Menu;
		case SDLK_0: return ImGuiKey_0;
		case SDLK_1: return ImGuiKey_1;
		case SDLK_2: return ImGuiKey_2;
		case SDLK_3: return ImGuiKey_3;
		case SDLK_4: return ImGuiKey_4;
		case SDLK_5: return ImGuiKey_5;
		case SDLK_6: return ImGuiKey_6;
		case SDLK_7: return ImGuiKey_7;
		case SDLK_8: return ImGuiKey_8;
		case SDLK_9: return ImGuiKey_9;
		case SDLK_a: return ImGuiKey_A;
		case SDLK_b: return ImGuiKey_B;
		case SDLK_c: return ImGuiKey_C;
		case SDLK_d: return ImGuiKey_D;
		case SDLK_e: return ImGuiKey_E;
		case SDLK_f: return ImGuiKey_F;
		case SDLK_g: return ImGuiKey_G;
		case SDLK_h: return ImGuiKey_H;
		case SDLK_i: return ImGuiKey_I;
		case SDLK_j: return ImGuiKey_J;
		case SDLK_k: return ImGuiKey_K;
		case SDLK_l: return ImGuiKey_L;
		case SDLK_m: return ImGuiKey_M;
		case SDLK_n: return ImGuiKey_N;
		case SDLK_o: return ImGuiKey_O;
		case SDLK_p: return ImGuiKey_P;
		case SDLK_q: return ImGuiKey_Q;
		case SDLK_r: return ImGuiKey_R;
		case SDLK_s: return ImGuiKey_S;
		case SDLK_t: return ImGuiKey_T;
		case SDLK_u: return ImGuiKey_U;
		case SDLK_v: return ImGuiKey_V;
		case SDLK_w: return ImGuiKey_W;
		case SDLK_x: return ImGuiKey_X;
		case SDLK_y: return ImGuiKey_Y;
		case SDLK_z: return ImGuiKey_Z;
		case SDLK_F1: return ImGuiKey_F1;
		case SDLK_F2: return ImGuiKey_F2;
		case SDLK_F3: return ImGuiKey_F3;
		case SDLK_F4: return ImGuiKey_F4;
		case SDLK_F5: return ImGuiKey_F5;
		case SDLK_F6: return ImGuiKey_F6;
		case SDLK_F7: return ImGuiKey_F7;
		case SDLK_F8: return ImGuiKey_F8;
		case SDLK_F9: return ImGuiKey_F9;
		case SDLK_F10: return ImGuiKey_F10;
		case SDLK_F11: return ImGuiKey_F11;
		case SDLK_F12: return ImGuiKey_F12;
		case SDLK_F13: return ImGuiKey_F13;
		case SDLK_F14: return ImGuiKey_F14;
		case SDLK_F15: return ImGuiKey_F15;
		case SDLK_F16: return ImGuiKey_F16;
		case SDLK_F17: return ImGuiKey_F17;
		case SDLK_F18: return ImGuiKey_F18;
		case SDLK_F19: return ImGuiKey_F19;
		case SDLK_F20: return ImGuiKey_F20;
		case SDLK_F21: return ImGuiKey_F21;
		case SDLK_F22: return ImGuiKey_F22;
		case SDLK_F23: return ImGuiKey_F23;
		case SDLK_F24: return ImGuiKey_F24;
		case SDLK_AC_BACK: return ImGuiKey_AppBack;
		case SDLK_AC_FORWARD: return ImGuiKey_AppForward;
	}
	return ImGuiKey_None;
}

ImVec4 clear_color(0.45f, 0.55f, 0.60f, .00f);
bool done = false;
char materialID = 0;
unsigned long long int iteration = 0;
bool play = false;

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

void show_material_selection_buttons(int ncols, vector<tuple<string, ImU32>> materials) {
	for (int i = 0; i < materials.size(); i++) {
		show_material_selection_button(
			i, get<0>(materials[i]), get<1>(materials[i])
		);
		if (i % ncols < ncols - 1)
			ImGui::SameLine();
	}
}

// Main code
int main(int argc, char* argv[])
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char *glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char *glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window *window = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// It would be better, to just leave the mouse absence as it is
	#ifdef __EMSCRIPTEN__
	// But we allow enabling it using args
	if(!args_set.count("--wheel"))
		SDL_EventState(SDL_MOUSEWHEEL, SDL_DISABLE);
	#endif

	// https://github.com/emscripten-ports/SDL2/issues/128
	typedef tuple<ImGuiIO*, SDL_Window*> filter_user_data_t;
	filter_user_data_t filter_user_data(&io, window);
	SDL_SetEventFilter([](void* userdata, SDL_Event *event){
		// Filtering the keyboard events, and handling one only
		// when a text input is active
		if(done) return 0;

		auto g = ImGui::GetCurrentContext();
		const filter_user_data_t* data = static_cast<filter_user_data_t*>(userdata);
		const ImGuiIO* io = get<0>(*data);
		SDL_Window* window = get<1>(*data);

		// Handling application quit
		if (event->type == SDL_QUIT) {
			done = true;
			return 1;
		}
		else if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_CLOSE && event->window.windowID == SDL_GetWindowID(window)) {
			done = true;
			return 1;
		}

		const bool process_result = ImGui_ImplSDL2_ProcessEvent(event);
		switch(event->type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				const ImGuiKey key = ImGui_ImplSDL2_KeycodeToImGuiKey(event->key.keysym.sym);

				// Key routing
				const ImGuiKeyOwnerData* owner_data = ImGui::GetKeyOwnerData(g, key);
				if(owner_data->OwnerCurr != ImGuiKeyOwner_None)
					return 1;
				
				// Shortcut routing
				const ImGuiKeyRoutingTable* rt = &g->KeysRoutingTable;
				if(rt->Index[key - ImGuiKey_NamedKey_BEGIN] != -1)
					return 1;
				
				break;
			}
			case SDL_TEXTINPUT:
			{
				if(io->WantTextInput)
					return 1;
				
				break;
			}
			default:
				return (int) process_result;
		}
		return 0;
	}, &filter_user_data);

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);


	// SIMULATION INIT
	auto air = phx::MaterialsList::addMaterial("breathable air", 1, PHX_MTYPE_GAS, phx::Color(0,0,0,0));
		air->isRemovable = true;
	auto sand = phx::MaterialsList::addMaterial("dry sand", 4, PHX_MTYPE_POD, phx::Color(255,225,180));
		sand->addColor(phx::Color(245,245,170));
		sand->addColor(phx::Color(190,175,170));
	auto water = phx::MaterialsList::addMaterial("liquid water", 3, PHX_MTYPE_LIQ, phx::Color(50,150,255));
		water->viscosity = 3000;
	auto metal = phx::MaterialsList::addMaterial("unknown metal", 5, PHX_MTYPE_SOL, phx::Color(170,160,170));
	auto toxic_gas = phx::MaterialsList::addMaterial("toxic gas", 0.4, PHX_MTYPE_GAS, phx::Color(155,255,0,60));
	auto acid = phx::MaterialsList::addMaterial("mysterious acid", 3, PHX_MTYPE_LIQ, phx::Color(100,255,100));
		acid->addReaction(metal, air, toxic_gas);
	auto bedrock = phx::MaterialsList::addMaterial("technical bedrock", 0, PHX_MTYPE_SOL, phx::Color(0,0,0,255)); // as for v1.0, a bedrock solid frame around the scene is suggested for multiple reasons. this is planned to be changed later

	// this is important. otherwise you're going to have a segfault right at the start
	phx::scene.fill(air);
	phx::scene.fillFrame(bedrock); // this is optional but suggested for now


	// Main loop
#ifdef EMSCRIPTEN_CODE
	emscripten_log(EM_LOG_CONSOLE, "The main loop definition begin!");
#endif

	auto a = SDL_GetTicks();

#ifdef __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!done)
#endif
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		
		// DOING SIMULATION TICKS
		if (play) {
			phx::scene.updateAll();
			iteration++;
		}


		// WINDOW RENDERING PIPELINE

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
			{ "breath\nable\n air", IM_COL32(0,0,0,0) },
			{ "dry \nsand", IM_COL32(255,225,180,255) },
			{ "liquid\n water", IM_COL32(50,150,255,255) },
			{ "unkn\nown \nmetal", IM_COL32(170,160,170,255) },
			{ "toxic\n gas", IM_COL32(155,255,0,60) },
			{ "myste\nrious\n acid", IM_COL32(100,255,100,255) },
			{ "techn\nical \nbedrock", IM_COL32(0,0,0,255) },
		});

		ImGui::End();

		// Displaying Simulation window
		stringstream ss;
		ss<<"Simulation - Iteration: "<<iteration;
		if (!play) ss<<" Paused ";
		ss << "###simulation_window";
		ImGui::Begin(ss.str().c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		const ImVec2 window_size = {
			(float) (phyxel_size * PHX_SCENE_SIZE_X) + 15,
			(float) (phyxel_size * PHX_SCENE_SIZE_Y) + 30
		};
		ImGui::SetWindowSize(window_size);

		const auto p = ImGui::GetCursorScreenPos();
		const auto cursor = ImGui::GetCursorPos();
		auto available_space = ImGui::GetContentRegionAvail();
		// https://github.com/ocornut/imgui/issues/3149
		ImGui::InvisibleButton("canvas", available_space,
			ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_AllowOverlap);
		ImGui::SetItemAllowOverlap();

		// Handling left click on the simulation field
		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			auto mousePos = ImGui::GetMousePos();
            phx::scene.setMaterial(
				(mousePos.x - p.x) / phyxel_size,
				(mousePos.y - p.y) / phyxel_size,
				phx::MaterialsList::get(materialID),
				rand()
			);
		}

		// Rendering the simulation
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		// drawing vertical lines
		const auto lines_color = IM_COL32(255, 255, 255, 50);
		for (size_t i = 0; i < PHX_VIEW_SIZE_X; i++)
			draw_list->AddLine(
				{ p.x + i*phyxel_size, p.y },
				{ p.x + i*phyxel_size, p.y + window_size.y },
				lines_color
			);
		
		// drawing horizontal ones
		for (size_t i = 0; i < PHX_VIEW_SIZE_Y; i++)
			draw_list->AddLine(
				{ p.x, p.y + i*phyxel_size },
				{ p.x + window_size.x, p.y + i*phyxel_size },
				lines_color
			);

		// Drawing phyxels
		for (size_t i = 0; i < PHX_SCENE_SIZE_X * PHX_SCENE_SIZE_Y; i++) {
			int x = i%PHX_SCENE_SIZE_X;
			int y = i/PHX_SCENE_SIZE_X;
            phx::Color c = phx::scene.getColor(i);
			draw_list->AddRectFilled(
				{ p.x + x*phyxel_size, p.y + y*phyxel_size },
				{ p.x + (x + 1)*phyxel_size, p.y + (y + 1)*phyxel_size },
				IM_COL32(c.r, c.g, c.b, c.a)
			);
        }


		ImGui::SetCursorPos(cursor);
		ImGui::End();

		//ImGui::getScroll
		//ImGui::GetScroll
		// ImGuiWindow* b_window = ImGui::GetCurrentWindow();
		// const Vec2 scroll = ImGui::GetCurrentWindow()->Scroll;
		// if(scroll.abs() > 0)
		// auto context = ImGui::GetCurrentContext();
		// field.set_debug_message(to_string((int) context->WheelingWindow));

		//display_control_window(field);
		
		const auto delta_ticks = DELTA_TICKS;
		auto b = SDL_GetTicks();
		if (b - a < delta_ticks) {
			SDL_Delay(delta_ticks + a - b);
		}
		a = b;

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#else
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
#endif

	return 0;
}