#include "MemoryInspector.h"
#include <sstream>
#include <iomanip>

using namespace chhelper::toStrings;
using chhelper::stringfuncs::isHexChar, chhelper::stringfuncs::hasIllegalHexChars;

namespace appwindows
{

    MemoryInspector::MemoryInspector()
    {
        memoryinspector::bookmark::LoadBookmarks();
        addBookmarkWindow = memoryinspector::bookmark::AddBookmarkWindow(&selectedAddress);
        memSearchWindow.p_memInspector = this;
    }

    void MemoryInspector::ShowWindow()
    {

        // Demonstrate the various window flags. Typically you would just use the default!
        static bool no_titlebar = false;
        static bool no_scrollbar = false;
        static bool no_menu = false;
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
        
        
       
        // Menu Bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Search"))
            {
                if (ImGui::MenuItem("Find"))
                {
                    memSearchWindow.p_open = true;
                    memSearchWindow.mode = SWM_FIND;
                }
                if (allowMemoryEditing)
                {
                    if (ImGui::MenuItem("Replace"))
                    {
                        memSearchWindow.p_open = true;
                        memSearchWindow.mode = SWM_REPLACE;
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Jump to"))
            {
                if (ImGui::MenuItem("Program Counter")) jumpToAddress(cpu.pc);
                if (ImGui::MenuItem("Stack Pointer")) jumpToAddress(cpu.sp);
                if (ImGui::BeginMenu("Bookmarks"))
                {
                    if (ImGui::MenuItem("Add Bookmark")) addBookmarkWindow.p_open = true;
                    
                    memoryinspector::bookmark::bookmarkMenu();
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // update child windows
        addBookmarkWindow.Update();
        memSearchWindow.Update();

        // jump if bookmarks menu is requesting it
        if (memoryinspector::bookmark::shouldMakeJump)
        {
            jumpToAddress(memoryinspector::bookmark::jumpToBookmarkAddr);
            memoryinspector::bookmark::shouldMakeJump = false;
        }

        // start address of the table must be low enough that the end of the memory (0xFFFF) is not before the end of the table.
        tableoffset = std::min(tableoffset, maxDisplayOffset);

        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, 480);

        uint16_t lastViewableAddress = tableoffset + (0x10 * GB_MEMORY_INSPECTOR_MEMAREA_ROWS) -1;
        
        // memory area table 
        if (ImGui::BeginTable("memory_inspector_memory_area", 17))
        {
            selectedAddrTags.clear();
            
            // add table headers
            {
                ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed);
                // Headers 00 to 0F inclusive
                for (uint8_t column = 0x00; column <= 0x0F; column++) ImGui::TableSetupColumn(hexToString(column, false).c_str(), ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableHeadersRow();
            }

            // add buttons for memory addresses
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

                    bool isSelectedCell = addressOfCell == selectedAddress;
                    bool isCellColoured = false;
                    bool isCellTextColoured = false;
                    bool hasTooltip = false;

                    setButtonStyle(addressOfCell, isCellColoured, isSelectedCell, isCellTextColoured, hasTooltip);
                    
                    if (isSelectedCell) selectedAddrTags = curButtonTooltips;
                    
                    if (ImGui::Button(hexToString(MemoryBus_read_byte(cpu.bus,addressOfCell),false).c_str(), ImVec2(18, 13))) selectedAddress = addressOfCell;
                    
                    popButtonStyle(isCellColoured, isCellTextColoured);

                    if (hasTooltip) addButtonTooltips(addressOfCell);

                    ImGui::PopStyleVar();
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }

        // scrolling buttons
        {
            if (ImGui::Button(" - 0x1000"))
            {
                tableoffset -= 0x1000;
                selectedAddress -= 0x1000;
            }
            ImGui::SameLine();
            if (ImGui::Button(" - 0x100"))
            {
                tableoffset -= 0x100;
                selectedAddress -= 0x100;
            }
            ImGui::SameLine();
            // - 0x10 button is unavailable at the start of the memory, as the memory table doesn't wrap
            if (tableoffset == 0x0000) { ImGui::Button("-------"); ImGui::SameLine(); }
            else if (ImGui::Button(" - 0x10"))
            {
                tableoffset -= 0x10;
                if (selectedAddress > tableoffset + 0xF0) selectedAddress -= 0x10;
            }
            ImGui::SameLine();

            
            ImGui::Text("    "); ImGui::SameLine(); // spacer

            // + 0x10 button is unavailable at the end of the memory, as the memory table doesn't wrap
            if (tableoffset == 0xff00) { ImGui::Button("-------"); ImGui::SameLine(); }
            else
            {
                if (ImGui::Button(" + 0x10"))
                {
                    tableoffset += 0x10;
                    if (selectedAddress < tableoffset) selectedAddress += 0x10;
                }
                ImGui::SameLine();
            }
            if (ImGui::Button(" + 0x100"))
            {
                tableoffset += 0x100;
                selectedAddress += 0x100;
            }
            ImGui::SameLine();
            if (ImGui::Button(" + 0x1000"))
            {
                tableoffset += 0x1000;
                selectedAddress += 0x1000;
            }
        }

        // navigation section, under table 
        {
            bool prevAllowMemEdit = allowMemoryEditing;
            ImGui::Checkbox("Allow editing of memory", &allowMemoryEditing);
            if (allowMemoryEditing && !prevAllowMemEdit) ImGui::OpenPopup("Allow memory editing?");
            if (ImGui::BeginPopupModal("Allow memory editing?"))
            {
                ImGui::TextWrapped("Are you sure you want to allow memory editing? This is only for advanced users, and is likely to cause undefined behaviour, crashes and loss of data.");
                if (ImGui::Button("Yes, I understand the risks")) ImGui::CloseCurrentPopup();
                ImGui::SameLine();
                if (ImGui::Button("No, take me back"))
                {
                    allowMemoryEditing = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::NewLine();
            // jump to address
            ImGui::Text("Jump to:"); 
            ImGui::Text("0x"); ImGui::SameLine();
            ImGui::PushItemWidth(40);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::InputText("##memory_ins_jump_to_input_text", jumpToText, 5); ImGui::SameLine();
            ImGui::PopItemWidth();
            if(ImGui::Button("Enter")) jumpToAddress(std::string(jumpToText));
            ImGui::PopStyleVar();
            // jump to useful points
            if (ImGui::Button("Program Counter")) jumpToAddress(cpu.pc);
            if (ImGui::Button("Stack Pointer")) jumpToAddress(cpu.sp);
        }


        // window right column
        ImGui::NextColumn();
        if (ImGui::Button("|<")) selectedAddress = 0x0000; ImGui::SameLine();
        if (ImGui::Button("<")) selectedAddress--; ImGui::SameLine();
        if (ImGui::Button(">")) selectedAddress++; ImGui::SameLine();
        if (ImGui::Button(">|")) selectedAddress = 0xFFFF;

        // if selected byte is not on screen / table
        if (selectedAddress < tableoffset || selectedAddress > lastViewableAddress)
        {
            tableoffset = selectedAddress & 0xFFF0;
        }
        
        ImGui::Text((std::string("Selected Address: ") + hexToString(selectedAddress)).c_str());
        ImGui::Text((std::string("Address Label: ") + getAddressLabel(selectedAddress)).c_str());

        // display value in different formats
        uint8_t selected_value = MemoryBus_read_byte(cpu.bus, selectedAddress);
        ImGui::Text("Value as:");
        ImGui::Text( ( std::string("Hex: ") + hexToString(selected_value) ).c_str());
        ImGui::Text((std::string("Dec: ") + std::to_string(selected_value)).c_str());

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

        ImGui::Text((  std::string("ASCII char: ") += (char)selected_value ).c_str());
        ImGui::Text((std::string("8-bit Instruction: ") + instructionByteToFullDetails(selected_value)).c_str());
        ImGui::Text((std::string("0xCB Prefixed Instruction: ") + instructionByteToFullDetails(selected_value,true)).c_str());

        if (selectedAddrTags.size() > 1) // ignore first tag, as this will always be "selected address"
        {
            ImGui::NewLine();
            ImGui::Text("Address Is:");
            for (auto i = selectedAddrTags.begin() +1; i != selectedAddrTags.end(); i++)
            {
                ImGui:: Text((*i).c_str());
            }
        }
     


        ImGui::End();
    }

    // as of yet unused
    void MemoryInspector::UpdateInspectorValues()
    {
        

    }

    
    void MemoryInspector::jumpToAddress(uint16_t address)
    {
        selectedAddress = address;
    }

    // does nothing if address string is invalid
    void MemoryInspector::jumpToAddress(std::string address)
    {
        for (auto i = address.begin(); i != address.end(); i++)
        {
            if (!isHexChar(*i)) return;
        }
        jumpToAddress(std::stoi(address, 0, 16));
    }


    std::string MemoryInspector::instructionByteToFullDetails(uint8_t byte, bool prefixed)
    {
        FullInstruction& ins = prefixed ? prefixedInstructions[byte] : eightBitInstructions[byte];
        return std::string(instructionDetailsText(ins) + " " + instructionOpText(ins,true) + " " + instructionOpText(ins, false));
    }

    std::string MemoryInspector::instructionDetailsText(FullInstruction& ins)
    {
        return std::string(magic_enum::enum_name(ins.instruction));
    }

    std::string MemoryInspector::instructionOpText(FullInstruction& ins,bool isOp1)
    {
        ArithmeticTarget& target = isOp1 ? ins.op1 : ins.op2;
        if (target != INVALID) return std::string(magic_enum::enum_name(target));
        else return std::string("---");
    }

   

    std::string MemoryInspector::getAddressLabel(uint16_t addr)
    {
        std::string label = "";
        if (rangeIncInc(0x0000, addr, 0x3FFF)) label += "ROM bank 00";
        else if (rangeIncInc(0x4000, addr, 0x7FFF)) label += "ROM bank X";
        else if (rangeIncInc(0x8000, addr, 0x9FFF)) label += "Video RAM";
        else if (rangeIncInc(0xA000, addr, 0xBFFF)) label += "External RAM (if on cart)";
        else if (rangeIncInc(0xC000, addr, 0xCFFF)) label += "Work RAM (WRAM)";
        else if (rangeIncInc(0xd000, addr, 0xdFFF)) label += "Work RAM 2 - Electric Boogaloo";
        else if (rangeIncInc(0xE000, addr, 0xFDFF)) label += "Echo RAM (use prohibited)";
        else if (rangeIncInc(0xFE00, addr, 0xFE9F)) label += "Object Attribute Memory";
        else if (rangeIncInc(0xFEA0, addr, 0xFEFF)) label += "Unusable (prohibited)";
        else if (rangeIncInc(0xFF00, addr, 0xFF7F)) label += "IO Registers";
        else if (rangeIncInc(0xFF80, addr, 0xFFFE)) label += "High RAM (HRAM)";
        else if ( 0xFFFF == addr ) label += "Interrupt Enable Register (IE)";
        else return "ERROR";
        
        return label;
    }

    void MemoryInspector::addButtonTooltips(uint16_t addr)
    {
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            for (auto i = curButtonTooltips.begin(); i != curButtonTooltips.end(); i++) {
                ImGui::TextUnformatted((*i).c_str());
            }
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // only isSelected has been set so far
    void MemoryInspector::setButtonStyle(uint16_t addr, bool& isColoured, bool& isSelected, bool& isTextColoured, bool& hasTooltip)
    {
        curButtonTooltips.clear();
        if (isSelected) setButtonStyle_SingleType(imgui_col_255_f(228, 90, 16, 255), ImVec4(0, 0, 0, 1),
            "Currently Selected Byte", isColoured, isTextColoured, hasTooltip);

        // if address is in search results
        std::vector<uint16_t>* p_searchResults = memSearchWindow.getPResults();
        if (p_searchResults->size() > 0)
        {
            if (std::find(p_searchResults->begin(), p_searchResults->end(), addr) != p_searchResults->end())
            {
                setButtonStyle_SingleType(imgui_col_255_f(255, 255, 10, 255), ImVec4(0, 0, 0, 1),
                    "Search Result", isColoured, isTextColoured, hasTooltip);
            }
        }

        if (addr == cpu.pc) setButtonStyle_SingleType(imgui_col_255_f(216, 160, 223, 255), ImVec4(0, 0, 0, 1), "Program Counter Location", isColoured, isTextColoured, hasTooltip);

        if (addr == cpu.sp) setButtonStyle_SingleType(imgui_col_255_f(99, 20, 245, 255), ImVec4(1, 1, 1, 1), "Stack Pointer Location", isColoured, isTextColoured, hasTooltip);

        if (addr > cpu.sp && addr != 0xFFFF) setButtonStyle_SingleType(imgui_col_255_f(164, 116, 252, 255), ImVec4(1, 1, 1, 1), "Part of stack", isColoured, isTextColoured, hasTooltip);
        
        // default tooltip (memory area descriptor)
        hasTooltip = true;
        curButtonTooltips.push_back(getAddressLabel(addr));
    }

    void MemoryInspector::popButtonStyle(bool& isColoured,bool& isTextColoured)
    {
        if (isColoured) ImGui::PopStyleColor(); 
        if (isTextColoured) ImGui::PopStyleColor();
    }

    void MemoryInspector::setButtonStyle_SingleType(ImVec4 buttonCol, ImVec4 textcol, std::string tooltip, bool& isCol, bool& isTextCol, bool& hasTooltip)
    {
        if (!isCol)
        {
            isCol = true;
            ImGui::PushStyleColor(ImGuiCol_Button, buttonCol);
            isTextCol = true;
            ImGui::PushStyleColor(ImGuiCol_Text, textcol);
        }
        if (tooltip != "")
        {
            hasTooltip = true;
            curButtonTooltips.push_back(tooltip);
        }
    }

  

   

    void MemorySearchWindow::validateSearchWindowInputString(std::string& str, uint8_t length)
    {
        if (str.size() > length) startAddrString.resize(length);
        // make sure strings are not empty, hasIllegalHexChars wont pick this up as a problem
        
        if (str == "")
        {
            std::string s;
            for (int i = 0; i < length; i++)
            {
                s += "0";
            }
            str = s;
        }
    
    }

    void MemorySearchWindow::ShowWindow()
    {
        if (p_memInspector == nullptr)
        {
            throw std::exception("missing reference to memory inspector!");
        }
        std::string windowTitle = mode == SWM_FIND ? "Find###meminsp_searchwindow" : "Replace###meminsp_searchwindow";

        if (!ImGui::Begin(windowTitle.c_str(), &p_open, 0))
        {
            ImGui::End();
            return;
        }
        
        // search mode selection
        if (p_memInspector->getAllowMemoryEditing())
        {
            ImGui::Text("Mode: ");
            ImGui::SameLine();
            if (ImGui::RadioButton("Find", mode == SWM_FIND))
            {
                mode = SWM_FIND;
                searchResults.clear();
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Replace", mode == SWM_REPLACE))
            {
                mode = SWM_REPLACE;
                searchResults.clear();
            }
        }
        else
        {
            mode = SWM_FIND;
        }

        ImGui::Text("Find Value: "); ImGui::SameLine();
        ImGui::InputText("###meminsp_search_value_input", &searchValueString);

        if (mode == SWM_REPLACE)
        {
            ImGui::Text("Replace with: "); ImGui::SameLine();
            ImGui::InputText("###meminsp_replace_value_input", &replaceValueString);
        }
        
        ImGui::Text("Area to search:"); ImGui::SameLine();
        ImGui::Combo("###meminsp_search_area_mode", &searchAreaMode, "All Memory\0Between Addresses (Inclusive)\0Until Address\0From Address\0\0");


        
        switch (searchAreaMode)
        {
            case 0: // all memory
                startAddrString = "0000";
                endAddrString = "FFFF";
                break;
            case 1: // between addresses inclusive
                ImGui::Text("Start Address:"); ImGui::SameLine();
                ImGui::InputText("###meminsp_search_start_Addr", &startAddrString);
                ImGui::Text("End Address:"); ImGui::SameLine();
                ImGui::InputText("###meminsp_search_end_Addr", &endAddrString);
                break;
            case 2: // until address
                ImGui::Text("End Address:"); ImGui::SameLine();
                ImGui::InputText("###meminsp_search_end_Addr", &endAddrString);
                break;
            case 3: // from address
                ImGui::Text("Start Address:"); ImGui::SameLine();
                ImGui::InputText("###meminsp_search_start_Addr", &startAddrString);
                break;

            default:
                throw std::exception("Invalid search area mode!");
                break;
        }


        validateSearchWindowInputString(endAddrString,4);
        validateSearchWindowInputString(startAddrString, 4);

        if (mode == SWM_REPLACE)
        {
            validateSearchWindowInputString(replaceValueString, 2);
            if (!hasIllegalHexChars(replaceValueString)) replaceValue = std::stoi(replaceValueString, 0, 16);
            else
            {
                replaceValue = 0x00;
                replaceValueString = "00";
            }
        }

        if (!hasIllegalHexChars(endAddrString)) searchAreaEndInc = std::stoi(endAddrString, 0, 16);
        else
        {
            searchAreaEndInc = 0xFFFF;
            endAddrString = "FFFF";
        }
        if (!hasIllegalHexChars(startAddrString)) searchAreaStartInc = std::stoi(startAddrString, 0, 16);
        else
        {
            searchAreaStartInc = 0x0000;
            startAddrString = "0000";
        }
        
        // search value
        validateSearchWindowInputString(searchValueString, 2);
        if (!hasIllegalHexChars(searchValueString)) searchValue = std::stoi(searchValueString, 0, 16);
        else
        {
            searchValue = 0x00;
            searchValueString = "00";
        }

        if (ImGui::Button(mode == SWM_FIND ? "Find###meminsp_search_confirmbutton" : "Replace###meminsp_search_confirmbutton"))
        {
            searchResults.clear();
            searchResults.reserve(65536); //TODO there must be some smarter way to do this? feels like a lot to reserve, when we probably wont be returning 65,536 results.

            // do the searching and or replacing
            for (unsigned int addr = searchAreaStartInc; addr <= searchAreaEndInc; addr++)
            {
                if (MemoryBus_read_byte(cpu.bus, (uint16_t)addr) == searchValue) searchResults.push_back((uint16_t)addr);
            }
            if (mode == SWM_REPLACE && p_memInspector->getAllowMemoryEditing())
            {
                for (const auto& addr : searchResults)
                {
                    MemoryBus_write_byte(cpu.bus, addr, replaceValue);
                }
            }
        }

        if (searchResults.size() > 0)
        {
            ImGui::SameLine();
            if (ImGui::Button("Clear Results"))
            {
                searchResults.clear();
            }
            std::string s = mode == SWM_FIND ? "Found " : "Found and replaced ";
            s += std::to_string(searchResults.size()) + " results.";
            ImGui::Text(s.c_str());
        }

        ImGui::End();
    }

    MemorySearchWindow::MemorySearchWindow()
    {
    
    }

}