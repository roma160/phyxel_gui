#pragma once

// From imgui.h "Helpers: ImVec2/ImVec4 operators"
static inline ImVec4 operator*(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }

// Defined to be able to put ImVec2ih into a std::set (otherwise have error C2676)
// https://stackoverflow.com/a/61455584/8302811
// https://stackoverflow.com/a/27894016/8302811
static inline bool operator< (const ImVec2ih& lhs, const ImVec2ih& rhs) {
    if (lhs.x == rhs.x) return lhs.y < rhs.y;
    return lhs.x < rhs.x;
}