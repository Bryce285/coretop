#include <ftxui/dom/elements.hpp>
#include <string>
#include <format>
#include <cmath>

#include "UI.hpp"
#include "CPU.hpp"
#include "memory.hpp"

UI::UI(size_t numCores)
{
    // temporary CPUCore object for initialization
    CPU::CPUCore tempCore;

    // 2d vector to store cpu util % gauges from the last 10 refreshes for each core
    UI::CPUGauges.resize(numCores);
    for (size_t i = 0; i < UI::CPUGauges.size(); ++i) {
        UI::CPUGauges[i].resize(NUM_GAUGES);
        UI::CPUGauges[i].assign(NUM_GAUGES, renderCPUCore(tempCore, 0.0f));
    }
}

ftxui::Element UI::renderCPUCore(CPU::CPUCore core, double memPressure)
{
    float coreUsage = static_cast<float>(core.usagePercent) / 100.0f;
    coreUsage = std::clamp(coreUsage, 0.0f, 1.0f);

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    if (memPressure <= 25.0f) {
        // green
        red = 46;
        green = 125;
        blue = 50;
    }
    else if (memPressure > 25.0f && memPressure <= 50.0f) {
        // yellow
        red = 249;
        green = 168;
        blue = 37;
    }
    else if (memPressure > 50.0f && memPressure <= 75.0f) {
        // orange
        red = 239;
        green = 108;
        blue = 0;
    }
    else if (memPressure > 75.0f && memPressure <= 100.0f) {
        // red
        red = 198;
        green = 40;
        blue = 40;
    }
    else {
        // grey
        // this indicates something went wrong calculating stressPercent
        red = 36;
        green = 31;
        blue = 30;
    }

    return  ftxui::vbox({
            ftxui::gaugeUp(coreUsage) 
            | ftxui::bgcolor(ftxui::Color::RGB(red, green, blue)) 
            | ftxui::flex
            });
}

ftxui::Element UI::renderMemory(Memory::MemInfo memInfo, Memory::VmStat vmStat, double memPressure, Memory::MemInfoDiff memInfoDiff, Memory::VmStatInstantDiff vmStatInstantDiff, float memPressureDiff)
{
    std::vector<ftxui::Element> elements;

    ftxui::Element label =  ftxui::vbox({
                            ftxui::hbox({
                            ftxui::bold(ftxui::text("Memory Stat")) | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::filler(),
                            ftxui::text("Value [instantaneous]") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::text("Change since last refresh") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)}),
                            ftxui::separator()});
    elements.push_back(label);

    ftxui::Element pressurePercent =    ftxui::hbox({
                                        ftxui::text("Pressure") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                        ftxui::filler(),
                                        ftxui::text(std::to_string(static_cast<int>(memPressure)) + " %") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                        ftxui::text(std::to_string(static_cast<int>(memPressureDiff)) + " %") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                                        }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(pressurePercent);

    ftxui::Element available =  ftxui::hbox({
                                ftxui::text("Available") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::filler(),
                                ftxui::text(std::to_string(memInfo.available) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::text(std::to_string(memInfoDiff.available) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                                });
    elements.push_back(available);

    ftxui::Element dirty =  ftxui::hbox({
                            ftxui::text("Dirty") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::filler(),
                            ftxui::text(std::to_string(memInfo.dirty) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::text(std::to_string(memInfoDiff.dirty) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                            }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(dirty);

    ftxui::Element writeback =  ftxui::hbox({
                                ftxui::text("Writeback") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::filler(),
                                ftxui::text(std::to_string(memInfo.writeback) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::text(std::to_string(memInfoDiff.writeback) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                                });
    elements.push_back(writeback);

    ftxui::Element cached = ftxui::hbox({
                            ftxui::text("Cached") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::filler(),
                            ftxui::text(std::to_string(memInfo.cached) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::text(std::to_string(memInfoDiff.cached) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                            }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(cached);

    ftxui::Element active = ftxui::hbox({
                            ftxui::text("Active") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::filler(),
                            ftxui::text(std::to_string(memInfo.active) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::text(std::to_string(memInfoDiff.active) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                            });
    elements.push_back(active);

    ftxui::Element pgFault =    ftxui::hbox({
                                ftxui::text("Page Faults") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::filler(),
                                ftxui::text(std::to_string(vmStat.pgFault) + " last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::text(std::to_string(vmStatInstantDiff.pgFault)) | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                                }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(pgFault);

    ftxui::Element pgMajFault = ftxui::hbox({
                                ftxui::text("Major Page Faults") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::filler(),
                                ftxui::text(std::to_string(vmStat.pgMajFault) + " last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::text(std::to_string(vmStatInstantDiff.pgMajFault)) | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                                });
    elements.push_back(pgMajFault);

    ftxui::Element pSwpIn = ftxui::hbox({
                            ftxui::text("Page Swap In") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::filler(),
                            ftxui::text(std::to_string(vmStat.pSwpIn) + " pages last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                            ftxui::text(std::to_string(vmStatInstantDiff.pSwpIn) + " pages") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                            }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(pSwpIn);

    ftxui::Element pSwpOut = ftxui::hbox({
                                ftxui::text("Page Swap Out") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::filler(),
                                ftxui::text(std::to_string(vmStat.pSwpOut) + " pages last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30),
                                ftxui::text(std::to_string(vmStatInstantDiff.pSwpOut) + " pages") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 30)
                                });
    elements.push_back(pSwpOut);

    ftxui::Element allMemory = ftxui::vbox(elements);

    return allMemory;
}

ftxui::Element UI::renderHeader(CPU::Time uptime, CPU::Time idleTime, Memory::MemUsage memoryData)
{
    std::string uptimeStr = std::format("{:03}:{:02}:{:02}:{:02}", uptime.days, uptime.hours, uptime.minutes, uptime.seconds);

    std::string idleTimeStr = std::format("{:03}:{:02}:{:02}:{:02}", idleTime.days, idleTime.hours, idleTime.minutes, idleTime.seconds);

    std::string totalStr = std::to_string(memoryData.total);
    std::string usageStr = std::to_string(memoryData.usage);
    double usagePercent = (static_cast<double>(memoryData.usage) / static_cast<double>(memoryData.total)) * 100.0;
    std::string usagePercentStr = std::to_string(static_cast<int>(usagePercent));

    return  ftxui::hbox({
            ftxui::text("System up time: " + uptimeStr),
            ftxui::filler(),
            ftxui::text("Memory Usage: [ " + usageStr + " / " + totalStr + " kB ] [ " + usagePercentStr + " % ]"),
            ftxui::filler(),
            ftxui::text("Time in idle process: " + idleTimeStr)
            });
}

ftxui::Element UI::renderAllCPU(std::vector<CPU::CPUCore> cores, CPU::Time uptime, CPU::Time idleTime, std::string cpuName, Memory::MemUsage memoryData, double memPressure, Memory::MemInfo memInfo, Memory::VmStat vmStat, Memory::MemInfoDiff memInfoDiff, Memory::VmStatInstantDiff vmStatInstantDiff, float memPressureDiff)
{
	const int GRAPH_WIDTH = NUM_GAUGES + 2;
    int graphsPerRow = std::max(1, screenWidth / GRAPH_WIDTH);
	UI::CPUGauges.resize(cores.size());

    // this vector will hold the graph element that will be created from each
    // nested vector in CPUGauges
    std::vector<ftxui::Element> graphs;
    const int MIN_GRAPH_HEIGHT = 15;

    for (size_t i = 0; i < cores.size(); ++i) {
        CPU::CPUCore curCore = cores[i];
        UI::CPUGauges[i].push_back(renderCPUCore(curCore, memPressure));
        UI::CPUGauges[i].erase(UI::CPUGauges[i].begin());

        auto utilGraph = ftxui::vbox({
                ftxui::hbox({
                        ftxui::text(curCore.id) | ftxui::bold | ftxui::center
                        }),
                
                ftxui::hbox(UI::CPUGauges[i])
                | ftxui::borderLight
                | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, MIN_GRAPH_HEIGHT)
				| ftxui::size(ftxui::WIDTH, ftxui::EQUAL, GRAPH_WIDTH)
                | ftxui::yflex,

                ftxui::hbox({
                        ftxui::text(std::to_string(curCore.usagePercent).substr(0, 5) + "%")
                        | ftxui::center
						}),

				ftxui::hbox({
					ftxui::text(std::to_string(curCore.frequency).substr(0, 3) + " GHz")
					| ftxui::center
					})
                }) | ftxui::yflex;

        graphs.push_back(utilGraph);
    }

	std::vector<ftxui::Element> graphRows;

	size_t total = graphs.size();

	size_t rows = (total + graphsPerRow - 1) / graphsPerRow;
	rows = std::max<size_t>(1, rows);

	size_t base = total / rows;
	size_t extra = total % rows;
	size_t index = 0;

	for (size_t i = 0; i < rows; i ++) {
		size_t count = base + (i < extra ? 1 : 0);

    	auto begin = graphs.begin() + index;
    	auto end   = begin + count;

		std::vector<ftxui::Element> row;
		for (auto it = begin; it != end; ++it) {
			row.push_back(*it);
			if (std::next(it) != end)
				row.push_back(ftxui::filler());
		}

		graphRows.push_back(ftxui::hbox(row) | ftxui::yflex);
		index += count;
	}

	auto allGraphs = ftxui::vbox(graphRows);
    auto header = renderHeader(uptime, idleTime, memoryData);
    auto memoryStats = renderMemory(memInfo, vmStat, memPressure, memInfoDiff, vmStatInstantDiff, memPressureDiff);
    
    auto document = ftxui::window(ftxui::text(cpuName) | ftxui::bold,
            ftxui::vbox({
                header,
                ftxui::separator(),
                allGraphs | ftxui::yflex,
                ftxui::separator(),
                memoryStats
                }) | ftxui::yflex);

    return document;
}
