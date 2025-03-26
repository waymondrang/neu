#include "Profiler.h"

#include <cassert>
#include <queue>

#include "core.h"

using namespace Neu;

Profiler::Profiler() {}

int Profiler::start(std::string sectionName) {
  if (intervalCount == 0) {
    ProfilerSection *childSection = new ProfilerSection();
    childSection->ID = currentID;
    childSection->name = sectionName;
    childSection->parentSection = lastSection;

    profilerData.push_back(childSection);

    // PUSH ONLY IF ROOT
    if (currentID > 0) profilerData[lastSection]->children.push_back(currentID);

    startTimes.push_back(std::chrono::high_resolution_clock::now());

    lastSection = currentID;

    // CHECK CURRENT SECTION IS LAST SECTION
    assert(currentID == profilerData.size() - 1);

    currentID++;

    return childSection->ID;
  }
}

void Profiler::stop(int sectionID) {
  if (intervalCount == 0) {
    if (sectionID != lastSection) {
      Logger::warn(
          "Mismatched stop category name with last section profiled\n");
      Logger::warn("\tCurrent function: %s\n",
                   profilerData[sectionID]->name.c_str());
      Logger::warn("\tLast function: %s\n",
                   profilerData[lastSection]->name.c_str());
    }

    if (endTimes.size() < startTimes.size()) endTimes.resize(startTimes.size());

    endTimes[sectionID] =
        std::chrono::high_resolution_clock::now() - startTimes[sectionID];

    lastSection = profilerData[sectionID]->parentSection;
  }
}

std::chrono::duration<double> Profiler::getTimeForSection(
    unsigned int sectionID) {
  if (sectionID >= endTimes.size()) return std::chrono::duration<double>(0);

  return endTimes[sectionID];
}

void Profiler::reset() {
  intervalCount++;

  if (intervalCount >= sampleInterval) {
    lastSection = 0;
    currentID = 0;
    intervalCount = 0;

    startTimes.clear();
    endTimes.clear();

    ///////////////////////////////
    // FREE PROFILER TREE MEMORY //
    ///////////////////////////////

    for (ProfilerSection *section : profilerData) free(section);

    profilerData.clear();
  }
}

////////////////////
// PROFILER AGENT //
////////////////////

ProfilerAgent::ProfilerAgent(std::string sectionName) {
  sectionID = Profiler::getInstance().start(sectionName);
}

ProfilerAgent::~ProfilerAgent() { Profiler::getInstance().stop(sectionID); }