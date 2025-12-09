#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <atomic>
#include <mutex>
#include <string>

#include "CPU.hpp"
#include "UI.hpp"
#include "memory.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/dom/node.hpp"
#include <ftxui/component/event.hpp>

CPU cpu;
UI ui(cpu.numCores);
Memory memory;

std::atomic<bool> quit = false;
std::vector<CPU::CPUCore> cores;
std::string cpuName = cpu.parseName();
std::mutex coresMutex;

void Update() {
    while (!quit) {

        std::vector<CPU::CPUCore> coresTemp;
        cpu.CPUUpdate(coresTemp);
        memory.memoryUpdate();

        {
            std::lock_guard<std::mutex> lock(coresMutex);
            cores = std::move(coresTemp);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{   
    std::thread updater(Update);
    std::string resetPosition;

    while (!quit) {

        {
            std::lock_guard<std::mutex> lock(coresMutex);
            if (cores.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
        }

        std::vector<CPU::CPUCore> localCopy;

        {
            std::lock_guard<std::mutex> lock(coresMutex);
            if (cores.empty()) continue;
            localCopy = cores;
        }
        
        ftxui::Element document = ui.renderAllCPU(localCopy, 
                cpu.uptime, cpu.idleTime, cpuName, 
                memory.memoryData, memory.pressurePercent, 
                memory.curMemInfo, memory.curVmStatInstant,
                memory.memInfoDiff, memory.vmStatInstantDiff,
                memory.pressurePercentDiff);

        auto screen = ftxui::Screen::Create(ftxui::Dimension::Full(), ftxui::Dimension::Full());
        screen.Clear();
        Render(screen, document);
        std::cout << resetPosition;
        screen.Print();
        std::cout.flush();
        resetPosition = screen.ResetPosition();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    quit = true;
    updater.join();

    return 0;
}
