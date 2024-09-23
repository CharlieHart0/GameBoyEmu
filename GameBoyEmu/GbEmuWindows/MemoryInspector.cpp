#include "MemoryInspector.h"
#include <sstream>
#include <iomanip>

namespace GbEmuWindows
{

    MemoryInspector::MemoryInspector()
    {
        
    }

    void MemoryInspector::ShowWindow()
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

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);


        if (!ImGui::Begin("Memory Inspector!", &p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, 480);

        /*ImGui::Text("test lol");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("test tooltip");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }*/
        

        if (ImGui::BeginTable("memory_inspector_memory_area", 17))
        {
            
            // add table headers
            {
                ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);

                ImGui::TableSetupColumn("00", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("01", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("02", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("03", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("04", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("05", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("06", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("07", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("08", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("09", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0A", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0B", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0C", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0D", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0E", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("0F", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
            }

            uint16_t tableoffset = 0x0000;

            for (int row = 0; row < GB_MEMORY_INSPECTOR_MEMAREA_ROWS; row++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text(hexToString((uint16_t)(tableoffset + (0x10 * row))).c_str());

                for (int insidecol = 0; insidecol < 16; insidecol++)
                {
                    uint16_t addressOfCell = tableoffset + (0x10 * row) + insidecol;
                    ImGui::TableNextColumn();
                    ImGui::PushID((std::string("memory_ins_button") + std::string(std::to_string((row * 16) + insidecol))).c_str());
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Button(hexToString(MemoryBus_read_byte(cpu.bus,addressOfCell),false).c_str(), ImVec2(18, 13))) selectedAddress = addressOfCell;
                    ImGui::PopStyleVar();
                    ImGui::PopID();
                }
                
            }

            ImGui::EndTable();
        }

        if (update_values) { UpdateInspectorValues(); }

        ImGui::NextColumn();
        uint8_t selected_value = MemoryBus_read_byte(cpu.bus, selectedAddress);
        ImGui::Text((std::string("Selected Address: ") + hexToString(selectedAddress)).c_str());
        ImGui::Text( ( std::string("Value (hex): ") + hexToString(selected_value) ).c_str());
        ImGui::Text((std::string("Value (dec): ") + std::to_string(selected_value)).c_str());
        if (ImGui::BeginTable("memory_inspector_value_binary", 9))
        {

            // add table headers
            {
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);

                ImGui::TableSetupColumn(".7", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".6", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".5", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".4", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".3", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".2", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn(".0", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
            }
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("(Binary)");
            for (int bit = 7; bit >= 0; bit--)
            {
                ImGui::TableNextColumn();
                ImGui::Text((selected_value >> bit) & 1 ? "1" : "0");
            }
            ImGui::EndTable();
        }
        ImGui::Text((  std::string("Value (ASCII char): ") += (char)selected_value ).c_str());




        ImGui::End();
    }


    void MemoryInspector::UpdateInspectorValues()
    {
        

    }

    std::string MemoryInspector::hexToString(uint16_t value, bool use0x)
    {
        std::stringstream ss;
        if (use0x) ss << "0x";
        ss << std::setw(4) << std::setfill('0') << std::hex << (int) value;
        return ss.str();
    }

    std::string MemoryInspector::hexToString(uint8_t value, bool use0x)
    {
        std::stringstream ss;
        if (use0x) ss << "0x";
        ss << std::setw(2) << std::setfill('0') << std::hex << (int) value;
        return ss.str();
    }

    std::string MemoryInspector::boolToString(bool value)
    {
        if (value) return "TRUE";
        return "FALSE";
    }

    bool MemoryInspector::getBitFromByte(uint8_t byte, uint8_t pos)
    {
        // TODO should i throw an error here? not sure
        if (pos > 7) return false;
        return (byte >> pos) & 1;
    }

   

}