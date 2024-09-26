#include "CPUInspector.h"
#include <sstream>
#include <iomanip>

using namespace toStrings;

namespace appwindows
{

    CPUInspector::CPUInspector()
    {
        for (int i = 0; i < magic_enum::enum_count<CPU_INSPECTOR_VALUE_FIELD>();i++)
        {
            cachedCPUValues[(CPU_INSPECTOR_VALUE_FIELD) i] = "ERR_NOT_READ_YET";
        }
    }

    void CPUInspector::ShowWindow()
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


        if (!ImGui::Begin("CPU Inspector!", &p_open, window_flags))
        {
            ImGui::End();
            return;
        }




        ImGui::Text(update_values ? "Current CPU Values" : "CPU Values (Old, may have changed)");

        if (update_values) { UpdateInspectorValues(); }

        ImGui::Text("Registers:");
        ImGui::Text((std::string("A = ") + cachedCPUValues[REG_A]).c_str());
        ImGui::Text((std::string("B = ") + cachedCPUValues[REG_B]).c_str());
        ImGui::Text((std::string("C = ") + cachedCPUValues[REG_C]).c_str());
        ImGui::Text((std::string("D = ") + cachedCPUValues[REG_D]).c_str());
        ImGui::Text((std::string("E = ") + cachedCPUValues[REG_E]).c_str());
        ImGui::Text((std::string("F (Flags Register) = ") + cachedCPUValues[REG_FLAG_REG]).c_str());
        ImGui::Text((std::string("H = ") + cachedCPUValues[REG_H]).c_str());
        ImGui::Text((std::string("L = ") + cachedCPUValues[REG_L]).c_str());
        ImGui::NewLine();
       
        ImGui::Text("Flags Register:");
        ImGui::Text((std::string("Zero = ") + cachedCPUValues[REG_FLAG_ZERO]).c_str());
        ImGui::Text((std::string("Subtract = ") + cachedCPUValues[REG_FLAG_SUBTRACT]).c_str());
        ImGui::Text((std::string("Half Carry = ") + cachedCPUValues[REG_FLAG_HALF_CARRY]).c_str());
        ImGui::Text((std::string("Carry = ") + cachedCPUValues[REG_FLAG_CARRY]).c_str());
        ImGui::NewLine();
        
        ImGui::Text("Other Values:");
        ImGui::Text((std::string("PC  = ") + cachedCPUValues[PC]).c_str());
        ImGui::Text((std::string("SP  = ") + cachedCPUValues[SP]).c_str());
        ImGui::Text((std::string("IME = ") + cachedCPUValues[IME]).c_str());
        ImGui::Text((std::string("Is Halted = ") + cachedCPUValues[IS_HALTED]).c_str());
        ImGui::NewLine();
        
        ImGui::Text("Last Called Instruction:");
        ImGui::Text((std::string("Instruction = ") + cachedCPUValues[LAST_INSTRUCTION]).c_str());
        ImGui::Text((std::string("OP1 = ") + cachedCPUValues[LAST_INSTRUCTION_OP1]).c_str());
        ImGui::Text((std::string("OP2 = ") + cachedCPUValues[LAST_INSTRUCTION_OP2]).c_str());
        ImGui::NewLine();


        ImGui::Checkbox("Update Values", &update_values);
        
        ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
        ImGui::PushItemWidth(100);
        ImGui::InputDouble("CPU Speed Multiplier", &cpu.desiredSpeedMultiplier);
        ImGui::PopItemWidth();
        if (cpu.desiredSpeedMultiplier > 1) ImGui::TextColored(ImVec4(0.921f, 0.007f, 0.007f, 1), "Time capped at 1x speed as the device's capacitors are insufficiently fluxed.");
        ImGui::PopTextWrapPos();
        
        if (ImGui::Button("1 x Speed (Realtime)")) cpu.desiredSpeedMultiplier = 1;
        if (ImGui::Button("1*10^-4 x Speed (Kinda Slow)")) cpu.desiredSpeedMultiplier = 0.0001;
        if (ImGui::Button("1*10^-5 x Speed (Slow)")) cpu.desiredSpeedMultiplier = 0.00001;
        if (ImGui::Button("1*10^-6 x Speed (Very Slow)")) cpu.desiredSpeedMultiplier = 0.000001;
        

#if defined (_DEBUG) && defined (DEBUG_GB_CPU_TIMER_INFO)
        ImGui::Text((std::string("Instructions Completed: ") + std::to_string(cpu.instructionsCompleted)).c_str());
        ImGui::Text((std::string("Total Duration (ns): ") + std::to_string(cpu.totalDuration)).c_str());
        if (cpu.instructionsCompleted != 0)
        {
            ImGui::Text((std::string("Avg Step Duration (ns): ") + std::to_string(cpu.totalDuration / cpu.instructionsCompleted)).c_str());
        }
#endif

        if (ImGui::Button("Tick CPU")) cpu.doOnlyOneInstruction = true;

        ImGui::End();
    }


    void CPUInspector::UpdateInspectorValues()
    {
        Registers& registers = cpu.registers;
        FlagsRegister& flagsregister = cpu.registers.f;

        cachedCPUValues[REG_A] = hexToString(registers.a);
        cachedCPUValues[REG_B] = hexToString(registers.b);
        cachedCPUValues[REG_C] = hexToString(registers.c);
        cachedCPUValues[REG_D] = hexToString(registers.d);
        cachedCPUValues[REG_E] = hexToString(registers.e);
        cachedCPUValues[REG_H] = hexToString(registers.h);
        cachedCPUValues[REG_L] = hexToString(registers.l);
        cachedCPUValues[REG_FLAG_REG] = hexToString(registers.f);

        cachedCPUValues[REG_FLAG_ZERO] = boolToString(flagsregister.zero);
        cachedCPUValues[REG_FLAG_SUBTRACT] = boolToString(flagsregister.subtract);
        cachedCPUValues[REG_FLAG_HALF_CARRY] = boolToString(flagsregister.half_carry);
        cachedCPUValues[REG_FLAG_CARRY] = boolToString(flagsregister.carry);

        cachedCPUValues[PC] = hexToString(cpu.pc);
        cachedCPUValues[SP] = hexToString(cpu.sp);

        cachedCPUValues[LAST_INSTRUCTION] = lastInstructionDetailsText();
        cachedCPUValues[LAST_INSTRUCTION_OP1] = lastInstructionOpText(true);
        cachedCPUValues[LAST_INSTRUCTION_OP2] = lastInstructionOpText(false);

        cachedCPUValues[IME] = boolToString(cpu.ime);
        cachedCPUValues[IS_HALTED] = boolToString(cpu.isHalted);

    }

    

    std::string CPUInspector::lastInstructionDetailsText()
    {
        if (cpu.lastCalledInstruction == nullptr) return std::string("None called yet");
        return std::string(magic_enum::enum_name(cpu.lastCalledInstruction->instruction));
    }

    std::string CPUInspector::lastInstructionOpText(bool isOp1)
    {
        if (cpu.lastCalledInstruction == nullptr) return std::string("None Yet");
        ArithmeticTarget& target = isOp1 ? cpu.lastCalledInstruction->op1 : cpu.lastCalledInstruction->op2;
        if (target != INVALID) return std::string(magic_enum::enum_name(target));
        else return std::string("---");
    }

}