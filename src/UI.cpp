#include <ftxui/dom/elements.hpp>
//#include <ftxui/util/color.hpp>
#include <string>
#include <format>
#include <cmath>

#include "UI.hpp"
#include "CPU.hpp"
#include "memory.hpp"

UI::UI(int numCores)
{
    // temporary CPUCore object for initialization
    CPU::CPUCore tempCore;

    // 2d vector to store cpu util % gauges from the last 10 refreshes for each core
    UI::CPUGauges.resize(numCores);
    int numGauges = 10;
    for (size_t i = 0; i < UI::CPUGauges.size(); ++i) {
        UI::CPUGauges[i].resize(numGauges);
        UI::CPUGauges[i].assign(numGauges, renderCPUCore(tempCore, 0.0f));
    }
}

ftxui::Element UI::renderCPUCore(CPU::CPUCore core, double memPressure)
{
    float coreUsage = static_cast<float>(core.usagePercent) / 100.0f;
    float usageDisplay = coreUsage * 100.0f;
    coreUsage = std::clamp(coreUsage, 0.0f, 1.0f);

    int red = 0;
    int green = 0;
    int blue = 0;
    if (memPressure <= 25.0f) {
        // green
        red = 63;
        green = 142;
        blue = 79;
    }
    else if (memPressure > 25.0f && memPressure <= 50.0f) {
        // yellow
        red = 179;
        green = 168;
        blue = 70;
    }
    else if (memPressure > 50.0f && memPressure <= 75.0f) {
        // orange
        red = 227;
        green = 158;
        blue = 84;
    }
    else if (memPressure > 75.0f && memPressure <= 100.0f) {
        // red
        red = 214;
        green = 77;
        blue = 77;
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
                            ftxui::bold(ftxui::text("Memory Stat")),
                            ftxui::filler(),
                            ftxui::text("Value [instantaneous]") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                            ftxui::text("Change since last refresh") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)}),
                            ftxui::separator()});
    elements.push_back(label);

    ftxui::Element pressurePercent =    ftxui::hbox({
                                        ftxui::text("Pressure"),
                                        ftxui::filler(),
                                        ftxui::text(std::to_string(static_cast<int>(memPressure)) + " %") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                                        ftxui::text(std::to_string(static_cast<int>(memPressureDiff)) + " %") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                                        }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(pressurePercent);

    ftxui::Element available =  ftxui::hbox({
                                ftxui::text("Available"),
                                ftxui::filler(),
                                ftxui::text(std::to_string(memInfo.available) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                                ftxui::text(std::to_string(memInfoDiff.available) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                                });
    elements.push_back(available);

    ftxui::Element dirty =  ftxui::hbox({
                            ftxui::text("Dirty"),
                            ftxui::filler(),
                            ftxui::text(std::to_string(memInfo.dirty) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                            ftxui::text(std::to_string(memInfoDiff.dirty) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                            }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(dirty);

    ftxui::Element writeback =  ftxui::hbox({
                                ftxui::text("Writeback"),
                                ftxui::filler(),
                                ftxui::text(std::to_string(memInfo.writeback) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                                ftxui::text(std::to_string(memInfoDiff.writeback) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                                });
    elements.push_back(writeback);

    ftxui::Element cached = ftxui::hbox({
                            ftxui::text("Cached"),
                            ftxui::filler(),
                            ftxui::text(std::to_string(memInfo.cached) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                            ftxui::text(std::to_string(memInfoDiff.cached) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                            }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(cached);

    ftxui::Element active = ftxui::hbox({
                            ftxui::text("Active"),
                            ftxui::filler(),
                            ftxui::text(std::to_string(memInfo.active) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                            ftxui::text(std::to_string(memInfoDiff.active) + " kB") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                            });
    elements.push_back(active);

    ftxui::Element pgFault =    ftxui::hbox({
                                ftxui::text("Page Faults"),
                                ftxui::filler(),
                                ftxui::text(std::to_string(vmStat.pgFault) + " last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                                ftxui::text(std::to_string(vmStatInstantDiff.pgFault) + " last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                                }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(pgFault);

    ftxui::Element pgMajFault = ftxui::hbox({
                                ftxui::text("Major Page Faults"),
                                ftxui::filler(),
                                ftxui::text(std::to_string(vmStat.pgMajFault) + " last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                                ftxui::text(std::to_string(vmStatInstantDiff.pgMajFault) + " last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                                });
    elements.push_back(pgMajFault);

    ftxui::Element pSwpIn = ftxui::hbox({
                            ftxui::text("Page Swap In"),
                            ftxui::filler(),
                            ftxui::text(std::to_string(vmStat.pSwpIn) + " pages last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                            ftxui::text(std::to_string(vmStatInstantDiff.pSwpIn) + " pages last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
                            }) | ftxui::bgcolor(ftxui::Color::RGB(20, 20, 20));
    elements.push_back(pSwpIn);

    ftxui::Element pSwpOut = ftxui::hbox({
                                ftxui::text("Page Swap Out"),
                                ftxui::filler(),
                                ftxui::text(std::to_string(vmStat.pSwpOut) + " pages last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50),
                                ftxui::text(std::to_string(vmStatInstantDiff.pSwpOut) + " pages last second") | size(ftxui::WIDTH, ftxui::GREATER_THAN, 50)
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
    UI::CPUGauges.resize(cores.size());

    // this vector will hold the graph element that will be created from each
    // nested vector in CPUGauges
    std::vector<ftxui::Element> graphs;
    int minGraphHeight = 15;

    for (size_t i = 0; i < cores.size(); ++i) {
        CPU::CPUCore curCore = cores[i];
        UI::CPUGauges[i].push_back(renderCPUCore(curCore, memPressure));
        UI::CPUGauges[i].erase(UI::CPUGauges[i].begin());

        auto utilGraph = ftxui::vbox({
                ftxui::hbox({
                        ftxui::text(curCore.id) | ftxui::center
                        }),
                
                ftxui::hbox(UI::CPUGauges[i])
                | ftxui::borderLight
                | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, minGraphHeight)
                | ftxui::yflex,

                ftxui::hbox({
                        ftxui::text(std::to_string(curCore.usagePercent).substr(0, 5) + "%")
                        | ftxui::center
                        })
                }) | ftxui::yflex;

        graphs.push_back(utilGraph);
    }

    std::vector<ftxui::Element> spacedGraphs;
    for (size_t i = 0; i < graphs.size(); ++i) {
        spacedGraphs.push_back(graphs[i]);
        if (i != graphs.size() - 1) {
            spacedGraphs.push_back(ftxui::filler());
        }
    }

    auto allGraphs = ftxui::hbox(spacedGraphs) | ftxui::xflex;
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
