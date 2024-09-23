#pragma once

#include "GbEmuWindow.h"
#include <iostream>
#include <string>

#define _CRT_SECURE_NO_WARNINGS

#include "../CPU.h"

#include "../Vendor/ImGui/imgui.h"
#include "../Vendor/ImGui-FileBrowser/imfilebrowser.h"

namespace GbEmuWindows
{
	class RomLoaderInfo : public GbEmuWindow
	{
	public:
		RomLoaderInfo();

		void ShowWindow();
		
	protected:
		ImGui::FileBrowser fileDialog;
		void addValidationErrorToStr(RomLoader_ValidationOutcome outcome, std::string& str);
	};
	

}