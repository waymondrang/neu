#ifndef PROFILER_H
#define PROFILER_H

#include <string>
#include <chrono>
#include <vector>
#include <tuple>

#define AVERAGE_CYCLES 10

#define PROFILE_START int profilerSectionID = Profiler::getInstance().start(__FUNCTION__)
#define PROFILE_STOP Profiler::getInstance().stop(profilerSectionID)

// note: assumes one top level profiler section
struct ProfilerSection
{
    int ID;
    std::string name;
    std::vector<int> children;
    int parentSection;
};

struct ProfilerTreeNode
{
    int ID;
    std::string name;
    std::chrono::duration<double> duration;
    std::vector<ProfilerTreeNode> children;
};

/**
 * simple, low-overhead profiler
 */
class Profiler
{
public:
    static Profiler &getInstance()
    {
        static Profiler instance;
        return instance;
    }

    Profiler();

    int start(std::string sectionName);
    void stop(int sectionID);
    std::chrono::duration<double> getTimeForSection(unsigned int sectionID);

    void reset();

    std::vector<ProfilerSection *> &getProfilerData() { return profilerData; }
    std::vector<std::chrono::duration<double>> &getEndTimes() { return endTimes; }

private:
    std::vector<ProfilerSection *> profilerData;
    int lastSection = 0;

    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> startTimes;
    std::vector<std::chrono::duration<double>> endTimes;

    const int sampleInterval = 15;
    int intervalCount = 0;

    int currentID = 0;
};

#endif