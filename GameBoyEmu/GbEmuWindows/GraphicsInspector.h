#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include "../CPU.h"
#include "../GBPallette.h"
#include "../ImGuiTexture.h"
#include "../Vendor/ImGui/imgui.h"
#include "GbEmuWindow.h"

namespace GbEmuWindows
{
	GBPalette palette_default({8,24,32,255}, {52, 104, 86,255}, {136, 192, 112,255}, {224, 248, 208,255});

	class GraphicsInspector : public GbEmuWindow
	{
	public:
		GraphicsInspector();

		void ShowWindow();

		void UpdateMemoryInspector();

		void CreateNewTileInspectorTextures();

		bool update_tiles = true;
	protected:
		
		std::array<ImGuiTexture, 128> tiles;
		
		//sample tile of a gameboy. not used atm
		uint8_t imgData[16]{0x3c,0x7e,0x42,0x42,0x42,0x42,0x42,0x42,0x7e,0x5e,0x7e,0x0a,0x7c,0x56,0x38,0x7c};
	};
        
        
		
	
	
}