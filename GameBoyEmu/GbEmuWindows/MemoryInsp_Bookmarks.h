#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <array>
#include "../Vendor/ImGui/imgui.h"

namespace appwindows
{
	namespace memoryinspector
	{
		namespace bookmark
		{
			const std::filesystem::path cwd = std::filesystem::current_path();
			 const std::filesystem::path mainDirectory = (std::filesystem::path) "UserConfigurable"/"MemoryInspector" / "Bookmarks";
			 const std::string fileSuffix = "GBADDR";

			 struct bookmark
			 {
				 uint16_t addr;   // eg 0xFF45
				 std::string label; // eg LYC
				 std::vector<std::string> path; // eg Display, LCD Status Registers
			 };
			 
			 struct bookmarkTreeNode
			 {
				 std::string nodeName;
				 std::map<std::string,bookmarkTreeNode> nodes;
				 std::vector<bookmark> bookmarks;
				 bookmarkTreeNode(std::string name)
				 {
					 nodeName = name;
				 }
			 };

			 bookmarkTreeNode rootNode("Bookmarks");

			 void LoadBookmarks();
			 void ClearBookmarks();
			 bool CreateBookmark(std::vector<std::string> path,uint16_t addr);
		}
	}
}