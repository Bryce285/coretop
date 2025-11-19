#include <ftxui/dom/elements.hpp>
#include <string>
#include <format>

#include "UI.hpp"
#include "CPU.hpp"

UI::UI(int numCores)
{
    // temporary CPUCore object for initialization
    CPU::CPUCore tempCore;

    // 2d vector to store cpu util % gauges from the last 20 refreshes for each core
    UI::CPUGauges.resize(numCores);
    int numGauges = 10;
    for (size_t i = 0; i < UI::CPUGauges.size(); ++i) {
	UI::CPUGauges[i].resize(numGauges);
	UI::CPUGauges[i].assign(numGauges, renderCPUCore(tempCore));
    }
}

ftxui::Element UI::renderCPUCore(CPU::CPUCore core)
{
    float coreUsage = static_cast<float>(core.usagePercent) / 100.0f;
    float usageDisplay = coreUsage * 100.0f;
    coreUsage = std::clamp(coreUsage, 0.0f, 1.0f);

    return  ftxui::vbox({
	    ftxui::gaugeUp(coreUsage) | ftxui::flex
    });
}

ftxui::Element UI::renderUptime(CPU::Time uptime, CPU::Time idleTime)
{
    std::string uptimeStr = std::format("{:03}:{:02}:{:02}:{:02}", uptime.days, uptime.hours, uptime.minutes, uptime.seconds);

    std::string idleTimeStr = std::format("{:03}:{:02}:{:02}:{:02}", idleTime.days, idleTime.hours, idleTime.minutes, idleTime.seconds);

    return  ftxui::hbox({
	    ftxui::text("System up time: " + uptimeStr),
	    ftxui::filler(),
	    ftxui::text("Time in idle process: " + idleTimeStr)
	    });
}

ftxui::Element UI::renderAllCPU(std::vector<CPU::CPUCore> cores, CPU::Time uptime, CPU::Time idleTime)
{
    UI::CPUGauges.resize(cores.size());

    // this vector will hold the graph element that will be created from each
    // nested vector in CPUGauges
    std::vector<ftxui::Element> graphs;
    int minGraphHeight = 15;

    for (size_t i = 0; i < cores.size(); ++i) {
	CPU::CPUCore curCore = cores[i];
	UI::CPUGauges[i].push_back(renderCPUCore(curCore));
	UI::CPUGauges[i].erase(UI::CPUGauges[i].begin());
	
	auto utilGraph = ftxui::vbox({
	    ftxui::hbox({
		ftxui::text(std::to_string(curCore.usagePercent).substr(0, 5) + "%")
		| ftxui::center
	    }),

	    ftxui::hbox(UI::CPUGauges[i])
	    | ftxui::border
	    | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, minGraphHeight),

	    ftxui::hbox({
		ftxui::text(curCore.id) | ftxui::center
	    })
	});
	graphs.push_back(utilGraph);
    }

    auto allGraphs = ftxui::hbox(graphs);

    auto uptimeElement = renderUptime(uptime, idleTime);

    auto document = ftxui::window(ftxui::text("CPU") | ftxui::center | ftxui::bold,
	    ftxui::vbox({
	    uptimeElement,
	    allGraphs
	    }));

    return document;
}
