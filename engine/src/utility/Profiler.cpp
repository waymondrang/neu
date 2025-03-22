#include "Profiler.h"

#include "../core.h"

#include <queue>
#include <cassert>

Profiler::Profiler() {}

int Profiler::start(std::string sectionName)
{
    if (intervalCount == 0)
    {
        ProfilerSection *childSection = new ProfilerSection();
        childSection->ID = currentID;
        childSection->name = sectionName;
        childSection->parentSection = lastSection;

        profilerData.push_back(childSection);

        if (currentID > 0)
            profilerData[lastSection]->children.push_back(currentID); // only push if not root

        startTimes.push_back(std::chrono::high_resolution_clock::now());

        lastSection = currentID;

        // debug: check currentID is size of startTimes
        // Logger::debug("currentID: %d, startTimes size: %d, profilerData size: %d\n", currentID, startTimes.size(), profilerData.size());
        // assert(currentID == startTimes.size() - 1);
        assert(currentID == profilerData.size() - 1);

        currentID++;

        return childSection->ID;
    }
}

void Profiler::stop(int sectionID)
{
    if (intervalCount == 0)
    {
        // Logger::debug("Profiler::stop begin");

        if (sectionID != lastSection)
        {
            Logger::warn("Mismatched stop category name with last section profiled\n");
            Logger::warn("\tCurrent function: %s\n", profilerData[sectionID]->name.c_str());
            Logger::warn("\tLast function: %s\n", profilerData[lastSection]->name.c_str());
        }

        if (endTimes.size() < startTimes.size())
            endTimes.resize(startTimes.size());

        // Logger::debug("Profiler::stop");

        endTimes[sectionID] = std::chrono::high_resolution_clock::now() - startTimes[sectionID];
        // startTimes.erase(startTimes.begin() + sectionID);

        lastSection = profilerData[sectionID]->parentSection;

        // Logger::debug("Profiler::stop finish");
    }
}

std::chrono::duration<double> Profiler::getTimeForSection(unsigned int sectionID)
{
    if (sectionID >= endTimes.size())
        return std::chrono::duration<double>(0);

    return endTimes[sectionID];
}

void Profiler::reset()
{
    intervalCount++;

    if (intervalCount >= sampleInterval)
    {
        lastSection = 0;
        currentID = 0;
        intervalCount = 0;

        startTimes.clear();
        endTimes.clear();

        ///////////////////////////////
        // FREE PROFILER TREE MEMORY //
        ///////////////////////////////

        for (ProfilerSection *section : profilerData)
            free(section);

        profilerData.clear();
    }
}