#include "RomInfoLoader.h"
#include <bitset>


namespace GbEmuWindows
{
    void RomLoaderInfo::Init(){
        
        fileDialog.SetTitle("Select ROM file");
        fileDialog.SetTypeFilters({".gb", ".rom",".h"}); // .h only for testing incorrect files

    }

    void RomLoaderInfo::ShowWindow()
        {

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


            if (!ImGui::Begin("Rom Loader!", &p_open, window_flags))
            {
                ImGui::End();
                return;
            }
            

            if (ImGui::Button("Load ROM file"))
                fileDialog.Open();
  

            fileDialog.Display();
            if (fileDialog.HasSelected())
            {
                cpu.romLoader.loadRomFromFile(fileDialog.GetSelected().string().c_str());
                fileDialog.ClearSelected();
            }

            // if rom loaded
            if (cpu.romLoader.rom.size() > 0)
            {
                
                if (cpu.romLoader.romValidationOutcome != 0)
                {
                    cpu.romLoader.modalTitle = "ROM validation failed!";
                    std::string s = "ROM Validation Issues:\n";
                    addValidationErrorToStr(MissingCompanyLogo, s);
                    addValidationErrorToStr(CartTypeErr, s);
                    addValidationErrorToStr(RomSize, s);
                    addValidationErrorToStr(RamSize, s);
                    addValidationErrorToStr(DestinationCode, s);
                    addValidationErrorToStr(HeaderChecksum, s);
                    cpu.romLoader.modalDesc = s;
                    cpu.romLoader.showModalWindow = true;
                    cpu.romLoader.deleteRom();
                }
                else
                {
                    ImGui::Text("ROM loaded! (this is a lie)");
                    //ImGui::Text(std::string(magic_enum::enum_name((RomLoader_ValidationOutcome) cpu.romLoader.romValidationOutcome)).c_str());
                }
            }


            ImGui::End();
        }

    void RomLoaderInfo::addValidationErrorToStr(RomLoader_ValidationOutcome outcome, std::string& str)
    {
        
        if ((outcome & cpu.romLoader.romValidationOutcome) == outcome)
        {
            str += std::string(magic_enum::enum_name(outcome)) + "\n";
        }
    
    }

    

}