#include "MemoryInsp_Bookmarks.h"


namespace appwindows
{
	namespace memoryinspector
	{
		namespace bookmark
		{
			bookmarkTreeNode rootNode("Bookmarks");

			void LoadBookmarks()
			{
				// clear existing bookmark directory tree
				ClearBookmarks();

				// then load em
				for (const std::filesystem::directory_entry& dir_entry :
					std::filesystem::recursive_directory_iterator(cwd/mainDirectory))
				{
					if (dir_entry.is_directory())
					{
						std::filesystem::path relPath = std::filesystem::relative(dir_entry.path(), cwd/mainDirectory);
						std::vector<std::string> pathStepsVec;
						for (auto i = relPath.begin(); i != relPath.end(); i++) pathStepsVec.push_back((*i).string());
						if (!nodeExists(pathStepsVec)) CreateNode(pathStepsVec);
						
						continue;
					}
					if (dir_entry.is_regular_file())
					{
						continue;
					}

				}
			}
			std::string fileOrDirName(std::filesystem::path path)
			{
				return std::filesystem::relative(path, path.parent_path()).string();
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
				bookmarkTreeNode& curNode = rootNode;
				for (auto i = dirPath.begin(); i != dirPath.end(); i++)
				{
					if (curNode.nodes.find(*i) == curNode.nodes.end()){
						return false;
					}
					curNode = curNode.nodes[*i];
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
				parentPath.reserve(dirPath.size() - 1);
				for (auto i = dirPath.begin(); std::distance(i, dirPath.end()) > 2; i++)
				{
					parentPath.push_back(*i);
				}
				if (!nodeExists(parentPath)) CreateNode(parentPath);
				bookmarkTreeNode& curNode = rootNode;
				for (auto i = dirPath.begin(); std::distance(i, dirPath.end()) > 2; i++)
				{
					curNode = curNode.nodes[*i];
				}
				curNode.nodes[dirPath[dirPath.size() - 1]] = bookmarkTreeNode(dirPath[dirPath.size() - 1]);
			};
		}
	}
}