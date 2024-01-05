#pragma once

// From imgui.h "Helpers: ImVec2/ImVec4 operators"
static inline ImVec4 operator*(const ImVec4& lhs, const float rhs) { return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs); }