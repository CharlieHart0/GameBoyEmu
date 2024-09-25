#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include "../CPU.h"
#include "../Instructions.h"
#include "../Vendor/ImGui/imgui.h"
#include "../ImGuiHelpers.h"
#include "GbEmuWindow.h"
#include "MemoryInsp_Bookmarks.h"
#include <filesystem>
#include <iostream>

#define GB_MEMORY_INSPECTOR_MEMAREA_ROWS 16

namespace appwindows
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

		void jumpToAddress(uint16_t address);
		void jumpToAddress(std::string address);
	protected:
		uint16_t selectedAddress = 0x0000;
		uint16_t tableoffset = 0x0000;
		char jumpToText[5] = "0000";
		std::vector<std::string> curButtonTooltips;
		std::vector<std::string> selectedAddrTags;

		// overflow expected and used here, maybe that isnt the right thing to do
		uint16_t maxDisplayOffset = 0 - (0x10 * GB_MEMORY_INSPECTOR_MEMAREA_ROWS);
		bool getBitFromByte(uint8_t byte, uint8_t pos);

		std::string instructionByteToFullDetails(uint8_t byte, bool prefixed = false);

		std::string instructionDetailsText(FullInstruction& ins);

		std::string instructionOpText(FullInstruction& ins,bool isOp1);

		bool isHexChar(char c);

		std::string getAddressLabel(uint16_t addr);
		bool rangeIncInc(uint16_t min, uint16_t v, uint16_t max) { return (min <= v) && (max >= v); }
		
		void addButtonTooltips(uint16_t addr);

		void setButtonStyle(uint16_t addr, bool&isColoured, bool&isSelected, bool& isTextColoured, bool& hasTooltip);

		void popButtonStyle(bool& isColoured,  bool& isTextColoured);

		// the name of this function is trash, make a better name
		void setButtonStyle_SingleType(ImVec4 buttonCol, ImVec4 textcol, std::string tooltip, bool& isCol, bool& isTextCol, bool& hasTooltip);
	};
}