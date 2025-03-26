#pragma once

#include <chrono>
#include <string>
#include <vector>

#define NEU_PROFILE_FUNCTION Neu::ProfilerAgent neuProfilerAgent(__FUNCTION__)

#define NEU_PROFILE_START          \
  const int neuProfilerSectionID = \
      Neu::Profiler::getInstance().start(__FUNCTION__)
#define NEU_PROFILE_STOP Neu::Profiler::getInstance().stop(neuProfilerSectionID)

#define NEU_PROFILE_START_CUSTOM(NAME)                                        \
  const int neuProfilerSectionID_##NAME = Neu::Profiler::getInstance().start( \
      std::string(__FUNCTION__) + "::" + std::string(#NAME))
#define NEU_PROFILE_STOP_CUSTOM(NAME) \
  Neu::Profiler::getInstance().stop(neuProfilerSectionID_##NAME)

namespace Neu {

struct ProfilerSection {
  int ID;
  std::string name;
  std::vector<int> children;
  int parentSection;
};

struct ProfilerTreeNode {
  int ID;
  std::string name;
  std::chrono::duration<double> duration;
  std::vector<ProfilerTreeNode> children;
};

class Profiler {
 public:
  static Profiler& getInstance() {
    static Profiler instance;
    return instance;
  }

  Profiler();

  int start(std::string sectionName);
  void stop(int sectionID);
  std::chrono::duration<double> getTimeForSection(unsigned int sectionID);

  void reset();

  std::vector<ProfilerSection*>& getProfilerData() { return profilerData; }
  std::vector<std::chrono::duration<double>>& getEndTimes() { return endTimes; }

 private:
  std::vector<ProfilerSection*> profilerData;
  int lastSection = 0;

  std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>
      startTimes;
  std::vector<std::chrono::duration<double>> endTimes;

  const int sampleInterval = 15;
  int intervalCount = 0;

  int currentID = 0;
};

class ProfilerAgent  // lol
{
 public:
  ProfilerAgent(std::string sectionName);
  ~ProfilerAgent();

 private:
  unsigned int sectionID;
};

};  // namespace Neu