#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include "../CPU.h"
#include "../Vendor/ImGui/imgui.h"
#include "GbEmuWindow.h"

namespace appwindows
{
	enum CPU_INSPECTOR_VALUE_FIELD
	{
		REG_A,
		REG_B,
		REG_C,
		REG_D,
		REG_E,
		REG_FLAG_REG,
		REG_FLAG_ZERO,
		REG_FLAG_SUBTRACT,
		REG_FLAG_HALF_CARRY,
		REG_FLAG_CARRY,
		REG_H,
		REG_L,
		PC,
		SP,
		LAST_INSTRUCTION,
		LAST_INSTRUCTION_OP1,
		LAST_INSTRUCTION_OP2,
		IME,
		IS_HALTED
	};

	class CPUInspector : public GbEmuWindow
	{
	public:
		CPUInspector();

		void ShowWindow();
		void UpdateInspectorValues();
		std::string hexToString(uint16_t value, bool use0x = true);
		std::string hexToString(uint8_t value, bool use0x = true);
		std::string boolToString(bool value);

		bool update_values = true;
	protected:

		std::map<CPU_INSPECTOR_VALUE_FIELD, std::string> cachedCPUValues;
		std::string lastInstructionDetailsText();
		std::string lastInstructionOpText(bool isOp1);
	};





}