#include <ftxui/dom/elements.hpp>
#include <string>
#include <format>

#include "CPU.hpp"

#pragma once

class UI
{
public:
    ftxui::Element renderAllCPU(std::vector<CPU::CPUCore> cores, CPU::Time uptime, CPU::Time idleTime);
private:
    // TODO - these can be static
    ftxui::Element renderCPUCore(CPU::CPUCore core);
    ftxui::Element renderUptime(CPU::Time uptime, CPU::Time idleTime);
};
