#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <array>
#include "../Vendor/ImGui/imgui.h"
#include "../Vendor/ImGui/misc/cpp/imgui_stdlib.h"
#include "GbEmuWindow.h"
#include "../Helper/toStrings.h"
#include "../Helper/stringfuncs.h"

class MemoryInspector;

namespace appwindows
{
	namespace memoryinspector
	{
		namespace bookmark
		{
			const std::filesystem::path cwd = std::filesystem::current_path();
			 const std::filesystem::path mainDirectory = (std::filesystem::path) "UserConfigurable"/"MemoryInspector" / "Bookmarks";
			 const std::string fileSuffix = ".GBADDR";
			 const std::string altFileSuffix = ".gbaddr";

			 struct Bookmark
			 {
				 uint16_t addr;   // eg 0xFF45
				 std::string label; // eg LYC
				 std::vector<std::string> path; // eg Display, LCD Status Registers
			 };
			 
			 class BookmarkTreeNode
			 {
			 public:
				 std::string nodeName;
				 std::map<std::string,BookmarkTreeNode> nodes;
				 std::vector<Bookmark> bookmarks;
				 BookmarkTreeNode(std::string name)
				 {
					 nodeName = name;
				 }
				 BookmarkTreeNode()
				 {
					 
				 }
			 };

			 class AddBookmarkWindow : public GbEmuWindow
			 {
			 protected:
				 void ShowWindow();
			 public:
				 uint16_t* memInspectorSelectedAddr = nullptr;
				 AddBookmarkWindow(uint16_t* addr);
				 AddBookmarkWindow();
			 };

			 extern BookmarkTreeNode rootNode;

			 void bookmarkMenu(bool root = true, BookmarkTreeNode* node = nullptr);

			 uint16_t jumpToBookmarkAddr = 0x0000;
			 bool shouldMakeJump = false;
			 std::string addBookmarkPath{"User Defined/bookmarkname"};
			 AddBookmarkWindow* addBookmarkWindow = nullptr;

			 std::string fileOrDirName(std::filesystem::path path);
			 void CreateNode(std::vector<std::string>dirPath);
			 bool nodeExists(std::vector<std::string>dirPath);
			 void LoadBookmarks();
			 void ClearBookmarks();
			 bool CreateBookmark(std::vector<std::string> path,uint16_t addr);
			 void AddBookmarkButton(bool& open, uint16_t* p_addr);

			 
		}
	}
}