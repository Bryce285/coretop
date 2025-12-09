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
    curVmStatInfo = parseVmStat();
    curMemInfo = parseMemory();
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

    pressurePercentDiff = static_cast<float>(pressurePercent - lastPressurePercent);
    
    long availableDiff = static_cast<long>(curMemInfo.available - lastMemInfo.available);
    long dirtyDiff = static_cast<long>(curMemInfo.dirty - lastMemInfo.dirty);
    long writebackDiff = static_cast<long>(curMemInfo.writeback - lastMemInfo.writeback);
    long cachedDiff = static_cast<long>(curMemInfo.cached - lastMemInfo.cached);
    long activeDiff = static_cast<long>(curMemInfo.active - lastMemInfo.active);

    memInfoDiff.available = availableDiff;
    memInfoDiff.dirty = dirtyDiff;
    memInfoDiff.writeback = writebackDiff;
    memInfoDiff.cached = cachedDiff;
    memInfoDiff.active = activeDiff;

    // the values parsed from vmstat are cumulative so we convert them to instantaneous
    unsigned long long pgFaultDiff = curVmStatInfo.pgFault - lastVmStatInfo.pgFault;
    unsigned long long pgMajFaultDiff = curVmStatInfo.pgMajFault - lastVmStatInfo.pgMajFault;
    unsigned long long pSwpInDiff = curVmStatInfo.pSwpIn - lastVmStatInfo.pSwpIn;
    unsigned long long pSwpOutDiff = curVmStatInfo.pSwpOut - lastVmStatInfo.pSwpOut;

    curVmStatInstant.pgFault = pgFaultDiff;
    curVmStatInstant.pgMajFault = pgMajFaultDiff;
    curVmStatInstant.pSwpIn = pSwpInDiff;
    curVmStatInstant.pSwpOut = pSwpOutDiff;

    long pgFaultInstantDiff = static_cast<long>(curVmStatInstant.pgFault - lastVmStatInstant.pgFault);
    long pgMajFaultInstantDiff = static_cast<long>(curVmStatInstant.pgMajFault - lastVmStatInstant.pgMajFault);
    long pSwpInInstantDiff = static_cast<long>(curVmStatInstant.pSwpIn - lastVmStatInstant.pSwpIn);
    long pSwpOutInstantDiff = static_cast<long>(curVmStatInstant.pSwpOut - lastVmStatInstant.pSwpOut);

    vmStatInstantDiff.pgFault = pgFaultInstantDiff;
    vmStatInstantDiff.pgMajFault = pgMajFaultInstantDiff;
    vmStatInstantDiff.pSwpIn = pSwpInInstantDiff;
    vmStatInstantDiff.pSwpOut = pSwpOutInstantDiff;

    // set last to current in preparation for next update
    lastMemInfo = curMemInfo;
    lastVmStatInfo = curVmStatInfo;
    lastVmStatInstant = curVmStatInstant;
    lastPressurePercent = pressurePercent;
}
