#include <string>
#include <iostream>
#include <vector>
#include <sstream>

#pragma once

class Memory
{
    public:
        Memory();

        struct MemUsage
        {
            unsigned long long total = 0;
            unsigned long long usage = 0;
        };

        struct MemInfo
        {
            unsigned long long total = 0;
            unsigned long long available = 0;
            unsigned long long dirty = 0;
            unsigned long long writeback = 0;
            unsigned long long cached = 0;
            unsigned long long active = 0;
        };

        struct VmStat
        {
            unsigned long long pgFault = 0;
            unsigned long long pgMajFault = 0;
            unsigned long long pSwpIn = 0;
            unsigned long long pSwpOut = 0;
        };
        
        struct Pressure
        {
            double avg10Some = 0.0;
            double avg10Full = 0.0;
        };
        
        struct MemInfoDiff
        {
            long available = 0;
            long dirty = 0;
            long writeback = 0;
            long cached = 0;
            long active = 0;
        };

        struct VmStatInstantDiff
        {
            long pgFault = 0;
            long pgMajFault = 0;
            long pSwpIn = 0;
            long pSwpOut = 0;
        };

        MemUsage memoryData;
        
        MemInfo curMemInfo;
        MemInfo lastMemInfo;
        MemInfoDiff memInfoDiff;

        VmStat curVmStatInfo;
        VmStat lastVmStatInfo;

        // to store instantaneous vmstat values
        VmStat curVmStatInstant;
        VmStat lastVmStatInstant;
        VmStatInstantDiff vmStatInstantDiff;
        
        double pressurePercent = 0.0;
        double lastPressurePercent = 0.0;
        float pressurePercentDiff = 0.0;

        void memoryUpdate();

    private:
        MemInfo parseMemory();
        VmStat parseVmStat();
        Pressure parsePressure();
};
