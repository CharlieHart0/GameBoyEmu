#include "MemoryInsp_Bookmarks.h"


namespace appwindows
{
	namespace memoryinspector
	{
		namespace bookmark
		{
			void LoadBookmarks()
			{
				ClearBookmarks();
				// load em
			}
			void ClearBookmarks()
			{
				//rootNode.nodes.clear();
				//rootNode.bookmarks.clear();
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
						if(!std::filesystem::is_directory(currentPath / *i))
						{
							std::filesystem::create_directory(currentPath / *i);
						}
						currentPath = currentPath / *i;
					}
				}

				return false;
			}
		}
	}
}