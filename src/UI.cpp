#include <ftxui/dom/elements.hpp>

#include "UI.hpp"
#include "CPU.hpp"

ftxui::Element UI::renderCPUCore(CPU::CPUCore core)
{
    float coreUsage = static_cast<float>(core.usagePercent) / 100.0f;
    float usageDisplay = coreUsage * 100.0f;
    coreUsage = std::clamp(coreUsage, 0.0f, 1.0f);

    return  ftxui::hbox({
	    ftxui::text(core.id) | ftxui::bold | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 8),
	    ftxui::separator(),

	    ftxui::gauge(coreUsage) | ftxui::flex,
	    ftxui::separator(),

	    ftxui::text(std::to_string(usageDisplay).substr(0, 5) + "%") | ftxui::center
    });
}

std::vector<ftxui::Element> UI::renderAllCPU(std::vector<CPU::CPUCore> cores)
{
    std::vector<ftxui::Element> CPUGauges;
    CPUGauges.resize(cores.size());

    for (size_t i = 0; i < cores.size(); ++i) {
	CPUGauges[i] = renderCPUCore(cores[i]);
    }

    return CPUGauges;
}
