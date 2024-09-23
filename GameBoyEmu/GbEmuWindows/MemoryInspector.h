#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include "../CPU.h"
#include "../Vendor/ImGui/imgui.h"
#include "GbEmuWindow.h"

#define GB_MEMORY_INSPECTOR_MEMAREA_ROWS 16

namespace GbEmuWindows
{


	class MemoryInspector : public GbEmuWindow
	{
	public:
		MemoryInspector();

		void ShowWindow();
		void UpdateInspectorValues();

		std::string hexToString(uint16_t value, bool use0x = true);
		std::string hexToString(uint8_t value, bool use0x = true);
		std::string boolToString(bool value);

		bool update_values = true;
	protected:
		uint16_t selectedAddress = 0x0000;
		bool getBitFromByte(uint8_t byte, uint8_t pos);

	};
}