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

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/dom/node.hpp"

CPU cpu;
UI ui;

std::atomic<bool> quit = false;
std::vector<CPU::CPUCore> cores;
std::mutex coresMutex;

void Update() {
    while (!quit) {
        std::vector<CPU::CPUCore> temp;
        cpu.CPUUpdate(temp); 

        {
            std::lock_guard<std::mutex> lock(coresMutex);
            cores = std::move(temp);  
	}

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{   
    std::thread updater(Update);
    auto screen = ftxui::Screen::Create(ftxui::Dimension::Full(), ftxui::Dimension::Fixed(30));
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
    
	std::vector<ftxui::Element> elements = ui.renderAllCPU(localCopy);
	auto document = ftxui::hbox(elements);	
	
	screen.Clear();
	Render(screen, document);
	std::cout << resetPosition;
	screen.Print();
	std::cout.flush();
	resetPosition = screen.ResetPosition();

	//if (std::cin.peek() == 'q') quit = true;

	std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    quit = true;
    updater.join();

    return 0;
}
