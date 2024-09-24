#pragma once
#include <cstdint>
#include "Vendor/ImGui/imgui.h"


ImVec4 imgui_col_255_f(float r, float g, float b, float a)
{
	return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}