#include <ftxui/dom/elements.hpp>
#include <string>
#include <format>

#include "CPU.hpp"
#include "memory.hpp"

#pragma once

class UI
{
    private:
        std::vector<std::vector<ftxui::Element>> CPUGauges;   

        ftxui::Element renderCPUCore(CPU::CPUCore core, double memPressure);
        ftxui::Element renderHeader(CPU::Time uptime, CPU::Time idleTime, Memory::MemUsage memoryData);

        ftxui::Element renderMemory(Memory::MemInfo memInfo, Memory::VmStat vmStat, double memPressure);

    public:
        UI(int numCores);

        ftxui::Element renderAllCPU(std::vector<CPU::CPUCore> cores, CPU::Time uptime, CPU::Time idleTime, std::string cpuName, Memory::MemUsage memoryData, double memPressure, Memory::MemInfo memInfo, Memory::VmStat vmStat);
};
