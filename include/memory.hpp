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

        MemUsage memoryData;
        
        // TODO - totals and stats from the last refresh period should
        // both be displayed on the UI, as well as an indication of whether
        // they increased or decreased since the last refresh
        MemInfo lastMemInfo;
        VmStat lastVmStatInfo;

        Pressure lastPressureInfo;
        double pressurePercent = 0.0;

        void memoryUpdate();

    private:
        MemInfo parseMemory();
        VmStat parseVmStat();
        Pressure parsePressure();
};
