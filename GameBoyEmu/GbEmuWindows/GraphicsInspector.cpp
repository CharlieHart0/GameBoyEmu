#include "GraphicsInspector.h"

namespace appwindows
{
    
        GraphicsInspector::GraphicsInspector()
        {
            CreateNewTileInspectorTextures();
        }
        void GraphicsInspector::ShowWindow()
        {

            // Demonstrate the various window flags. Typically you would just use the default!
            static bool no_titlebar = false;
            static bool no_scrollbar = false;
            static bool no_menu = true;
            static bool no_move = false;
            static bool no_resize = true;
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

            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);


            if (!ImGui::Begin("Graphics Inspector!", &p_open, window_flags))
            {
                ImGui::End();
                return;
            }


            ImGui::Text("Graphics memory:");

            ImGui::Columns(16, NULL,false);
            ImGui::Separator();

            if(update_tiles){ UpdateMemoryInspector(); }

            for (int i = 0; i < 128; i++)
            {
                ImGui::Image((void*) (intptr_t) tiles[i].mImgTex, ImVec2(tiles[i].mImgWidth * 4, tiles[i].mImgHeight * 4));
                ImGui::NextColumn();
            }
            ImGui::Columns(1, NULL);
            ImGui::NextColumn();

            ImGui::Checkbox("Update Tiles", &update_tiles);

            ImGui::End();
        }


        void GraphicsInspector::UpdateMemoryInspector()
        {
            for (std::array<ImGuiTexture, 128>::iterator i = tiles.begin(); i < tiles.end(); i++)
            {
                (*i).deleteTexture();
            }
            CreateNewTileInspectorTextures();
        }


        void GraphicsInspector::CreateNewTileInspectorTextures()
        {
            for (int i = 0; i < 128; i++)
            {
                tiles[i] = ImGuiTexture(&cpu.bus.memory[0] + 0x8000 + (i * 16), 8, 8, &palette_default);
            }
        }
    
}