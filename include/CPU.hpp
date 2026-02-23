#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#pragma once

class CPU
{
    public:
        CPU();

        struct CPUCore
        {
            std::string id;
            unsigned long long user = 0;
            unsigned long long nice = 0;
            unsigned long long system = 0; 
            unsigned long long idle = 0; 
            unsigned long long iowait = 0; 
            unsigned long long irq = 0; 
            unsigned long long softirq = 0; 
            unsigned long long steal = 0; 
            unsigned long long guest = 0; 
            unsigned long long guest_nice = 0;

            double usagePercent = 0.0;
            float frequency = 0.0f; 
        };

        struct Time
        {
            unsigned long long days = 0;
            unsigned long long hours = 0;
            unsigned long long minutes = 0;
            unsigned long long seconds = 0;
        };

        Time uptime;
        Time idleTime;

        std::string parseName();
        void CPUUpdate(std::vector<CPUCore>& cores);

        size_t numCores = 0;

    private:
        struct UptimeData
        {
            long double up = 0.0;
            long double idle = 0.0;
        };

        std::vector<CPUCore> parseCores();
        std::vector<CPUCore> coresLastCycle;
        UptimeData parseUptime();

        unsigned long long getIdle(CPUCore core);
        unsigned long long getTotal(CPUCore core);
        double getUsagePercent(unsigned long long idlePrev, unsigned long long idleCur, unsigned long long totalPrev, unsigned long long totalCur);

        void updateCores(std::vector<CPUCore>& cores);
        void updateUptime();

        Time secondsToTime(unsigned long long seconds);
};
