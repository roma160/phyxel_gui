#pragma once

#include "includes.h"
#include "static_variables.hpp"

using namespace std;

struct PhyxelWindowRenderData {
    string title;
    ImU32 phyxels_colors[PHX_SCENE_SIZE_X][PHX_SCENE_SIZE_Y];
};

class PhyxelWindow {
private:
    const string label;

    PhyxelWindowRenderData (*get_render_data)();
    void (*on_click)(float x, float y);

public:
    bool use_grid = true;

    /// @param on_click_callback Handles the left mouse click for the given coordinates IN THE WINDOW (not phyxel coordinates)
    // https://stackoverflow.com/a/55885752/8302811
    PhyxelWindow(
        string label,
        PhyxelWindowRenderData (*render_data_getter)(),
        void (*on_click_callback)(float x, float y)
    ): label("###" + label) {
        this->get_render_data = render_data_getter;
        this->on_click = on_click_callback;
    }

    void render() {
        auto render_data = get_render_data();

        stringstream ss;
        ss << render_data.title << label;
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
            on_click(mousePos.x - p.x, mousePos.y - p.y);
        }

        // Rendering the simulation
        ImDrawList *draw_list = ImGui::GetWindowDrawList();

        if (use_grid) {
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
        }

        // Drawing phyxels
        for (size_t i = 0; i < PHX_SCENE_SIZE_X * PHX_SCENE_SIZE_Y; i++)
        {
            int x = i % PHX_SCENE_SIZE_X;
            int y = i / PHX_SCENE_SIZE_X;
            draw_list->AddRectFilled(
                {p.x + x * phyxel_size, p.y + y * phyxel_size},
                {p.x + (x + 1) * phyxel_size, p.y + (y + 1) * phyxel_size},
                render_data.phyxels_colors[x][y]
            );
        }

        ImGui::SetCursorPos(cursor);
        ImGui::End();
    }
};