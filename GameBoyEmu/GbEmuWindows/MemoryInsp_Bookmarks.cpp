#include "MemoryInsp_Bookmarks.h"
#include "MemoryInspector.h"

namespace stdfs = std::filesystem;
using stringfuncs::split, std::vector, std::string;

namespace appwindows
{
	namespace memoryinspector
	{
		namespace bookmark
		{
			BookmarkTreeNode rootNode("Bookmarks");

			void LoadBookmarks()
			{
				// clear existing bookmark directory tree
				ClearBookmarks();

				// then load them
				for (const stdfs::directory_entry& dir_entry :
					stdfs::recursive_directory_iterator(cwd/mainDirectory))
				{
					// path from bookmarks directory to the current file
					stdfs::path relPath = stdfs::relative(dir_entry.path(), cwd / mainDirectory);
					std::vector<std::string> pathParts;
					for (auto i = relPath.begin(); i != relPath.end(); i++) pathParts.push_back((*i).string());

					if (dir_entry.is_directory())
					{
						if (!nodeExists(pathParts)) CreateNode(pathParts);
						continue;
					}
					if (dir_entry.is_regular_file())
					{
						// check the file extension is correct
						if (dir_entry.path().extension().string() != fileSuffix && 
							dir_entry.path().extension().string() != altFileSuffix) continue;
						// remove file name from relative bookmark path to get the directories leading to it
						pathParts.pop_back();

						// make sure node bookmark should be at exists
						if (!nodeExists(pathParts)) CreateNode(pathParts);

						// find node that bookmark should be at
						BookmarkTreeNode* curNode = &rootNode;
						for (auto i = pathParts.begin(); i != pathParts.end(); i++) curNode = &(curNode->nodes[*i]);

						uint16_t address = 0x0000;
						// read file for bookmark address
						std::ifstream bookmarkFile(dir_entry.path(), std::ios::binary);
						bookmarkFile.seekg(0, std::ios::end);
						if (bookmarkFile.tellg() < 2) { continue; } // file needs to be at least 2 bytes long
						bookmarkFile.seekg(0, std::ios::beg);
						std::array<char, 2> bytes;
						bookmarkFile.read((char*) &bytes, 2); // address is uint16_t, so ignore anything past first 2 bytes
						bookmarkFile.close();

						// put the read bytes into the address
						address |= ((uint16_t) bytes[0]) << 8;
						address |= bytes[1];

						// add bookmark with address and name to the correct location within the bookmark tree
						curNode->bookmarks.push_back(Bookmark(address, dir_entry.path().filename().string(), pathParts));
						continue;
					}
				}
			}

			//returns the final part of a filepath, for example C:/Users/JSmith/Documents/document.txt -> document.txt
			// and C:/Users/JSmith/Documents -> Documents
			std::string fileOrDirName(stdfs::path path)
			{
				return stdfs::relative(path, path.parent_path()).string();
			}

			void bookmarkMenu(bool root, BookmarkTreeNode* node)
			{
				if (!root && node == nullptr) return;
				if (root) node = &rootNode; 

				// add menu things for each directory 
				for (auto i = node->nodes.begin(); i != node->nodes.end(); i++)
				{
					// run this function recursively on each directory
					if (ImGui::BeginMenu(i->first.c_str()))
					{
						bookmarkMenu(false, &(i->second));
					
						ImGui::EndMenu();
					}
				}
					
				
				// add options for each bookmarked address 
				for (auto i = node->bookmarks.begin(); i != node->bookmarks.end(); i++)
				{
					if (ImGui::MenuItem((toStrings::hexToString(i->addr)+std::string(" - ") + i->label).c_str(), NULL))
					{
						shouldMakeJump = true;
						jumpToBookmarkAddr = i->addr;
					}
				}
			}

			void ClearBookmarks()
			{
				rootNode.nodes.clear();
				rootNode.bookmarks.clear();
			}

			bool CreateBookmark(std::vector<std::string> path, uint16_t addr)
			{
				stdfs::path currentPath = cwd / mainDirectory;
				for (auto i = path.begin(); i != path.end(); i++)
				{
					if (std::distance(i, path.end()) == 1) // if at file name
					{
						currentPath = currentPath / (*i + fileSuffix);
						std::ofstream outfile(currentPath);
						if (outfile.fail()) // eg. user has entered illegal characters
						{
							outfile.close();
							return false;
						}
						std::array<char, 2> buf{};
						buf[0] = (addr >> 8) & 0xFF;
						buf[1] = addr & 0xFF;
						outfile.write(buf.data(), buf.size());
						outfile.close();
					}
					else // at a directory
					{
						// create directory if does not exist
						try
						{
							if (!stdfs::is_directory(currentPath / *i)) stdfs::create_directory(currentPath / *i);
						}
						catch(stdfs::filesystem_error const& ex){
							return false; // eg. user has entered illegal characters
						}
						currentPath = currentPath / *i;
					}
				}
				LoadBookmarks();
				return true;
			}

			void AddBookmarkButton(bool& open, uint16_t* p_addr)
			{
				vector<string> path = split(addBookmarkPath, "/");
				open = !CreateBookmark(path, *p_addr);
			}
			
			// dirpath example - display, lcd control registers
			bool nodeExists(std::vector<std::string> dirPath)
			{
				BookmarkTreeNode* curNode = &rootNode;
				for (auto i = dirPath.begin(); i != dirPath.end(); i++)
				{
					if (curNode->nodes.find(*i) == curNode->nodes.end()){
						return false;
					}
					curNode = &(curNode->nodes[*i]);
					if (std::distance(i, dirPath.end()) == 1) // if final element
					{
						return true;
					}
				}
				return false;
			}

			void CreateNode(std::vector<std::string>dirPath)
			{
				if (nodeExists(dirPath)) return;
				std::vector<std::string> parentPath;
				if (dirPath.size() > 1)
				{
					parentPath.reserve(dirPath.size() - 1);
					for (auto i = dirPath.begin(); std::distance(i, dirPath.end()) > 1; i++) parentPath.push_back(*i);
					if (!nodeExists(parentPath)) CreateNode(parentPath);
				}
				BookmarkTreeNode* curNode = &rootNode;
				for (auto i = dirPath.begin(); std::distance(i, dirPath.end()) > 1; i++) curNode = &(curNode->nodes[*i]);
				curNode->nodes[dirPath[dirPath.size() - 1]] = BookmarkTreeNode(dirPath[dirPath.size() - 1]);
			};
			
			
			void AddBookmarkWindow::ShowWindow()
			{
				if (memInspectorSelectedAddr == nullptr) { return; }

				
				// Demonstrate the various window flags. Typically you would just use the default!
				static bool no_titlebar = false;
				static bool no_scrollbar = false;
				static bool no_menu = true;
				static bool no_move = false;
				static bool no_resize = false;
				static bool no_collapse = false;
				static bool no_close = false;
				static bool no_nav = false;
				static bool no_background = false;
				static bool no_bring_to_front = false;
				static bool unsaved_document = false;

				ImGuiWindowFlags window_flags = 0;
				if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
				if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
				if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
				if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
				if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
				if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
				if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
				if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
				if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
				if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
				if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

				if (!ImGui::Begin("Add Bookmarked Address", &p_open, window_flags))
				{
					ImGui::End();
					return;
				}
				

				ImGui::Text("Add Bookmark at location: "); ImGui::SameLine();
				ImGui::Text(toStrings::hexToString(*memInspectorSelectedAddr).c_str());

				ImGui::PushItemWidth(ImGui::GetWindowWidth()-20);
				ImGui::InputText(".gbaddr##meminsp_bookmark_add_input", &addBookmarkPath);
				ImGui::PopItemWidth();

				if (ImGui::Button("Add Bookmark")) AddBookmarkButton(p_open,memInspectorSelectedAddr);

				
				ImGui::End();
			}

			AddBookmarkWindow::AddBookmarkWindow(uint16_t* addr)
			{
				if (addBookmarkWindow != nullptr)
				{
					throw std::exception("Two of this class created!");
				}
				addBookmarkWindow = this;
				memInspectorSelectedAddr = addr;
			}
			AddBookmarkWindow::AddBookmarkWindow()
			{
				
			}
}
	}
}