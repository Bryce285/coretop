#include "CPU.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

CPU::CPU()
{
    std::vector<CPU::CPUCore> coresData = CPU::parseCores();
    CPU::numCores = coresData.size();

    // get initial values for all cores
    CPU::coresLastCycle = coresData;
}

std::vector<CPU::CPUCore> CPU::parseCores()
{
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/stat");
    }

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
	
		// sysfs file for system-wide cpu doesn't exist so don't try to open it
		if (core.id != "cpu") {	
			std::string path = "/sys/devices/system/cpu/" + core.id + "/cpufreq/scaling_cur_freq";
			std::ifstream freqFile(path);
			if (!file.is_open()) {
				throw std::runtime_error("Failed to open " + path);
			}

			long double freqkHz;
			freqFile >> freqkHz;
			float freqGHz = static_cast<float>(freqkHz / 1000000.0L);
			core.frequency = freqGHz;

			freqFile.close();
		}

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

void CPU::updateCores(std::vector<CPU::CPUCore>& cores)
{
    std::vector<CPUCore> cur = parseCores();

    if (coresLastCycle.empty() || coresLastCycle.size() != cur.size()) {
        coresLastCycle = cur;
        cores = cur;
        return;
    }

	// sysfs doesn't inlcude system-wide frequency so we calculate it separately
	float sysFreq = 0.0f;
	float allCoresUtil = 0.0f;

    // calculate usage percent for each core
    // use coresLastCycle for the calculations
    for (size_t i = 0; i < cur.size(); ++i) {
        unsigned long long idleCur = getIdle(cur[i]);
        unsigned long long totalCur = getTotal(cur[i]);

        unsigned long long idlePrev = getIdle(coresLastCycle[i]);
        unsigned long long totalPrev = getTotal(coresLastCycle[i]);

        double usage = getUsagePercent(idlePrev, idleCur, totalPrev, totalCur);

		cur[i].usagePercent = usage;

		if (cur[i].id != "cpu") {
			float usageScaled = usage / 100.0f;
			sysFreq += cur[i].frequency * usageScaled;
			allCoresUtil += usageScaled;
		}
    }

	for (size_t i = 0; i < cur.size(); ++i) {
		if (cur[i].id == "cpu") {
			if (allCoresUtil > 0.0f) 
				cur[i].frequency = sysFreq / allCoresUtil;
			else 
				cur[i].frequency = 0;
			break;
		}
	}

    cores = cur;
    coresLastCycle = std::move(cur);
}

CPU::UptimeData CPU::parseUptime()
{
    std::ifstream file("/proc/uptime");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/uptime");
    }

    CPU::UptimeData uptimeData;

    file >> uptimeData.up >> uptimeData.idle;

    return uptimeData;
}

void CPU::updateUptime()
{
    CPU::UptimeData cur = parseUptime();

    unsigned long long uptimeRaw = static_cast<unsigned long long>(cur.up);
    unsigned long long idleTimeRaw = static_cast<unsigned long long>(cur.idle);

    uptime = secondsToTime(uptimeRaw);
    idleTime = secondsToTime(idleTimeRaw);
}

CPU::Time CPU::secondsToTime(unsigned long long seconds)
{
    CPU::Time time;

    time.days = seconds / 86400;
    time.hours = (seconds % 86400) / 3600;
    time.minutes = (seconds % 3600) / 60;
    time.seconds = seconds % 60;

    return time;
}

std::string CPU::parseName()
{
    std::ifstream file("/proc/cpuinfo");
    if (!file.is_open()){
        throw std::runtime_error("Failed to open /proc/cpuinfo");
    }

    std::string cpuName;
    std::string line;

    while (std::getline(file, line)){
        if (line.rfind("model name", 0) == 0){
            size_t pos = line.find(':');
            if (pos != std::string::npos){
                cpuName = line.substr(pos + 1);
                cpuName.erase(0, cpuName.find_first_not_of("\t"));
            }

            break;
        }
    }

    return cpuName;
}

// call all update functions at once from here
void CPU::CPUUpdate(std::vector<CPUCore>& cores)
{
    updateCores(cores);
    updateUptime();
}
