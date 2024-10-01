#pragma once


#define _CRT_SECURE_NO_WARNINGS
#include "../GameBoyEmu/Vendor/ImGui/imgui.h"
#include "../GameBoyEmu/Vendor/ImGui/backends/imgui_impl_sdl2.h"
#include "../GameBoyEmu/Vendor/ImGui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <vector>
#include "A:/Program Files/SDL2/include/SDL.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>

#include "ImGuiTexture.h"
#include "CPU.h"
#include "PPU.h"
#include "Instructions.h"

#include "GbEmuWindows/GraphicsInspector.h"
#include "GbEmuWindows/RomInfoLoader.h"
#include "GbEmuWindows/CPUInspector.h"
#include "GbEmuWindows/MemoryInspector.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "A:/Program Files/SDL2/include/SDL_opengl.h"
#endif

extern std::mutex cpuAccessMutex;
std::atomic<bool> run_gameboy_cpu_atomic = false;
std::atomic<bool> run_gameboy_ppu_atomic = true;
void main_CPU_step();
void main_run_PPU();

