#include "MemoryInsp_Bookmarks.h"


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

				// then load em
				for (const std::filesystem::directory_entry& dir_entry :
					std::filesystem::recursive_directory_iterator(cwd/mainDirectory))
				{
					std::filesystem::path relPath = std::filesystem::relative(dir_entry.path(), cwd / mainDirectory);
					std::vector<std::string> pathStepsVec;
					for (auto i = relPath.begin(); i != relPath.end(); i++) pathStepsVec.push_back((*i).string());
					if (dir_entry.is_directory())
					{
						if (!nodeExists(pathStepsVec)) CreateNode(pathStepsVec);
						continue;
					}
					if (dir_entry.is_regular_file())
					{
						// check the file extension is correct
						if (dir_entry.path().extension().string() != fileSuffix && 
							dir_entry.path().extension().string() != altFileSuffix) continue;
						// remove file name from relative bookmark path to get the directories leading to it
						pathStepsVec.pop_back();

						// make sure node bookmark should be at exists
						if (!nodeExists(pathStepsVec)) CreateNode(pathStepsVec);

						// find node that bookmark should be at
						BookmarkTreeNode* curNode = &rootNode;
						for (auto i = pathStepsVec.begin(); i != pathStepsVec.end(); i++) curNode = &(curNode->nodes[*i]);

						uint16_t address = 0x0000;
						// read file for bookmark address
						std::ifstream bookmarkFile(dir_entry.path(), std::ios::binary);
						bookmarkFile.seekg(0, std::ios::end);
						if (bookmarkFile.tellg() < 2) { continue; }
						bookmarkFile.seekg(0, std::ios::beg);
						std::array<char, 2> bytes;
						bookmarkFile.read((char*) &bytes, 2);
						bookmarkFile.close();

						address |= ((uint16_t) bytes[0]) << 8;
						address |= bytes[1];

						curNode->bookmarks.push_back(Bookmark(address, dir_entry.path().filename().string(), pathStepsVec));
						continue;
					}
				}
			}
			std::string fileOrDirName(std::filesystem::path path)
			{
				return std::filesystem::relative(path, path.parent_path()).string();
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
				std::filesystem::path currentPath = cwd / mainDirectory;
				for (auto i = path.begin(); i != path.end(); i++)
				{
					if (std::distance(i, path.end()) == 1) // if at file name
					{
						currentPath = currentPath / (*i + "." + fileSuffix);
						std::ofstream outfile(currentPath);
						std::array<char, 2> buf{};
						buf[0] = (addr >> 8) & 0xFF;
						buf[1] = addr & 0xFF;
						outfile.write(buf.data(), buf.size());
						outfile.close();
					}
					else // at a directory
					{
						if (!std::filesystem::is_directory(currentPath / *i))
						{
							std::filesystem::create_directory(currentPath / *i);
						}
						currentPath = currentPath / *i;
					}
				}

				return false;
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
					for (auto i = dirPath.begin(); std::distance(i, dirPath.end()) > 1; i++)
					{
						parentPath.push_back(*i);
					}
					if (!nodeExists(parentPath)) CreateNode(parentPath);
				}
				BookmarkTreeNode* curNode = &rootNode;
				for (auto i = dirPath.begin(); std::distance(i, dirPath.end()) > 1; i++)
				{
					curNode = &(curNode->nodes[*i]);
				}
				curNode->nodes[dirPath[dirPath.size() - 1]] = BookmarkTreeNode(dirPath[dirPath.size() - 1]);
			};
		}
	}
}