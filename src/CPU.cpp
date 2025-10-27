#include "CPU.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

//#define DEBUG_PRINTS

CPU::CPU()
{
    // get initial values for all cores
    CPU::coresLastCycle = CPU::parseCores();
}

std::vector<CPU::CPUCore> CPU::parseCores()
{
    std::ifstream file("/proc/stat");
    std::string line;
    std::vector<CPUCore> cores;

    while (std::getline(file, line)){
	if (line.rfind("cpu", 0) != 0) break;

	std::istringstream iss(line);
	CPUCore core;
	
	if (!(iss >> core.id)) continue;

	unsigned long long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0; 

	iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

	core.user = user;
	core.nice = nice;
	core.system = system;
	core.idle = idle;
	core.iowait = iowait;
	core.irq = irq;
	core.softirq = softirq;
	core.steal = steal;
	core.guest = guest;
	core.guest_nice = guest_nice;

	cores.push_back(core);
    }
    return cores;    
}

unsigned long long CPU::getIdle(CPU::CPUCore core)
{
    unsigned long long idleAggregate = core.idle + core.iowait;
    return idleAggregate;
}

unsigned long long CPU::getTotal(CPU::CPUCore core)
{
    unsigned long long total = core.user + core.nice + core.system + core.idle + core.iowait + core.irq + core.softirq + core.steal;
    return total;
}

double CPU::getUsagePercent(unsigned long long idlePrev, unsigned long long idleCur, unsigned long long totalPrev, unsigned long long totalCur)
{
    double idleDelta = static_cast<double>(idleCur) - static_cast<double>(idlePrev);
    double totalDelta = static_cast<double>(totalCur) - static_cast<double>(totalPrev);

    double usagePercent;

    if (totalDelta <= 0.0) {
	usagePercent = 0.0;
    }
    else
	usagePercent = std::clamp((1.0 - (idleDelta / totalDelta)) * 100.0, 0.0, 100.0);

    return usagePercent;
}

void CPU::CPUUpdate(std::vector<CPU::CPUCore>& cores)
{
    std::vector<CPUCore> cur = parseCores();

    if (coresLastCycle.empty() || coresLastCycle.size() != cur.size()) {
	coresLastCycle = cur;
	cores = cur;
	return;
    }

    // calculate usage percent for each core
    // use coresLastCycle for the calculations
    for (size_t i = 0; i < cur.size(); ++i) {
	unsigned long long idleCur = getIdle(cur[i]);
	unsigned long long totalCur = getTotal(cur[i]);

	unsigned long long idlePrev = getIdle(coresLastCycle[i]);
	unsigned long long totalPrev = getTotal(coresLastCycle[i]);
	
	double usage = getUsagePercent(idlePrev, idleCur, totalPrev, totalCur);

#ifdef DEBUG_PRINTS
	std::cout << cur[i].id
	<< " idleDelta=" << (idleCur - idlePrev)
        << " totalDelta=" << (totalCur - totalPrev)
        << " usage=" << usage << "%\n";
#endif

	cur[i].usagePercent = usage;
    }
    
    cores = cur;
    coresLastCycle = std::move(cur);
}
