#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include "../CPU.h"
#include "../Instructions.h"
#include "../Vendor/ImGui/imgui.h"
#include "../ImGuiHelpers.h"
#include "GbEmuWindow.h"
#include "MemoryInsp_Bookmarks.h"
#include "../Helper/toStrings.h"
#include <filesystem>
#include <iostream>

#define GB_MEMORY_INSPECTOR_MEMAREA_ROWS 16

namespace appwindows
{
	
	enum SearchWindowMode
	{
		SWM_FIND,
		SWM_REPLACE
	};

	class MemoryInspector;

	class MemorySearchWindow : public GbEmuWindow
	{
	protected:
		void ShowWindow();
	public:
		SearchWindowMode mode = SWM_FIND;
		MemorySearchWindow();
		appwindows::MemoryInspector* p_memInspector = nullptr;
	};

	class MemoryInspector : public GbEmuWindow
	{
	
	protected:
		uint16_t selectedAddress = 0x0000;
		// address table starts at
		uint16_t tableoffset = 0x0000;
		// char buf for jump to address input
		char jumpToText[5] = "0000";
		std::vector<std::string> curButtonTooltips;
		// tags are the tooltips of the currently selected address, and are to be displayed in the inspector.
		std::vector<std::string> selectedAddrTags;

		memoryinspector::bookmark::AddBookmarkWindow addBookmarkWindow;
		MemorySearchWindow memSearchWindow;

		bool allowMemoryEditing = false;
		

		// table start address that puts final memory byte (0xFFFF) at end of table
		// overflow expected and used here, maybe that isnt the right thing to do
		uint16_t maxDisplayOffset = 0 - (0x10 * GB_MEMORY_INSPECTOR_MEMAREA_ROWS);
		

		std::string instructionByteToFullDetails(uint8_t byte, bool prefixed = false);

		std::string instructionDetailsText(FullInstruction& ins);

		std::string instructionOpText(FullInstruction& ins,bool isOp1);
		// returns true if character is 0-9, a-f or A-F
		bool isHexChar(char c);

		// label of the broad memory region of an address
		std::string getAddressLabel(uint16_t addr);
		bool rangeIncInc(uint16_t min, uint16_t v, uint16_t max) { return (min <= v) && (max >= v); }
		
		void addButtonTooltips(uint16_t addr);

		void setButtonStyle(uint16_t addr, bool&isColoured, bool&isSelected, bool& isTextColoured, bool& hasTooltip);

		void popButtonStyle(bool& isColoured,  bool& isTextColoured);

		// the name of this function is trash, make a better name
		void setButtonStyle_SingleType(ImVec4 buttonCol, ImVec4 textcol, std::string tooltip, bool& isCol, bool& isTextCol, bool& hasTooltip);

	public:
		MemoryInspector();

		void ShowWindow();
		void UpdateInspectorValues();
		void jumpToAddress(uint16_t address);
		void jumpToAddress(std::string address);
		uint16_t getSelectedAddress() { return selectedAddress; }
		bool getAllowMemoryEditing() { return allowMemoryEditing; }

		bool update_values = true;
		
	};

	
}