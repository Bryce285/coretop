#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>

#include "memory.hpp"

Memory::Memory()
{
    // TODO - may need to explicitly set lastMemInfo, lastVmStatInfo, lastPressureInfo
    // and maybe calculate memoryData from here initially
    memoryUpdate();
}

Memory::MemInfo Memory::parseMemory()
{
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/meminfo");
    }

    std::string line;
    MemInfo memInfo;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string label;
        unsigned long long value;
        std::string unit;
        
        if (line.rfind("MemTotal:", 0) == 0) {
            iss >> label >> value >> unit;
            memInfo.total = value;
        }
        else if (line.rfind("MemAvailable:", 0) == 0) {
            iss >> label >> value >> unit;
            memInfo.available = value;
        }
        else if (line.rfind("Cached:", 0) == 0) {
            iss >> label >> value >> unit;
            memInfo.cached = value;
        }
        else if (line.rfind("Active:", 0) == 0) {
            iss >> label >> value >> unit;
            memInfo.active = value;
        }
        else if (line.rfind("Dirty:", 0) == 0) {
            iss >> label >> value >> unit;
            memInfo.dirty = value;
        }
        else if (line.rfind("Writeback:", 0) == 0) {
            iss >> label >> value >> unit;
            memInfo.writeback = value;
        }
    }

    return memInfo;
}

Memory::VmStat Memory::parseVmStat()
{
    std::ifstream file("/proc/vmstat");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/vmstat");
    }

    std::string line;
    VmStat vmStat;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string label;
        unsigned long long value;
        
        if (line.rfind("pgfault", 0) == 0) {
            iss >> label >> value;
            vmStat.pgFault = value;
        }
        else if (line.rfind("pgmajfault", 0) == 0) {
            iss >> label >> value;
            vmStat.pgMajFault = value;
        }
        else if (line.rfind("pswpin", 0) == 0) {
            iss >> label >> value;
            vmStat.pSwpIn = value;
        }
        else if (line.rfind("pswpout", 0) == 0) {
            iss >> label >> value;
            vmStat.pSwpOut = value;
        }
    }

    return vmStat;
}

Memory::Pressure Memory::parsePressure()
{
    std::ifstream file("/proc/pressure/memory");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open /proc/pressure/memory");
    }

    std::string line;
    Pressure pressure;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string label;
        std::string avg10;
        std::string avg60;
        std::string avg300;
        std::string total;
        
        if (line.rfind("some", 0) == 0) {
            iss >> label >> avg10 >> avg60 >> avg300 >> total;

            avg10 = avg10.substr(avg10.find('=') + 1);
            pressure.avg10Some = std::stod(avg10);
        }
        else if (line.rfind("full", 0) == 0) {
            iss >> label >> avg10 >> avg60 >> avg300 >> total;

            avg10 = avg10.substr(avg10.find('=') + 1);
            pressure.avg10Full = std::stod(avg10);
        }
    }

    return pressure;
}

void Memory::memoryUpdate()
{
    VmStat curVmStatInfo = parseVmStat();
    MemInfo curMemInfo = parseMemory();
    Pressure curPressureInfo = parsePressure();
    unsigned long long total = curMemInfo.total;
    unsigned long long available = curMemInfo.available;
    
    const double SOME_MAX_STALL_PERCENT = 10.0;
    const double FULL_MAX_STALL_PERCENT = 2.0;
    const double FULL_WEIGHT = 0.7;
    const double SOME_WEIGHT = 0.3;

    double someNormalized = std::clamp(curPressureInfo.avg10Some / SOME_MAX_STALL_PERCENT, 0.0, 1.0);
    double fullNormalized = std::clamp(curPressureInfo.avg10Full / FULL_MAX_STALL_PERCENT, 0.0, 1.0);
    double pressure = std::clamp(FULL_WEIGHT * fullNormalized + SOME_WEIGHT * someNormalized, 0.0, 1.0);

    pressurePercent = std::round(pressure * 100.0);
 
    if (total == 0 || total < available) {
        memoryData.total = 0;
        memoryData.usage = 0;
        return;
    } 
    else {
        unsigned long long usage = total - available;
        memoryData.total = total;
        memoryData.usage = usage;
    }

    // set last to current in preparation for next update
    lastMemInfo = curMemInfo;
    lastVmStatInfo = curVmStatInfo;
    lastPressureInfo = curPressureInfo;
}
