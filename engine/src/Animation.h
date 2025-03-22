#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <tuple>
#include <string>

enum class InterpolationType
{
    FLAT,
    LINEAR,
    SMOOTH,
    CUSTOM_SLOPE // where tangents are specified
};

struct Keyframe
{
    float time;
    float value;
    InterpolationType tangentInType;
    InterpolationType tangentOutType;
    float tangentIn;
    float tangentOut;
};

enum class ExtrapolationType
{
    CONSTANT,
    LINEAR,
    CYCLE,
    CYCLE_OFFSET,
    BOUNCE
};

struct Channel
{
    ExtrapolationType extrapolationInType;
    ExtrapolationType extrapolationOutType;
    std::vector<Keyframe> keyframes;
};

class Animation
{
public:
    Animation();
    ~Animation();

    bool load(const char *filename);
    void evaluate(float wallTime);

    void setName(std::string name) { this->name = name; }

    std::string getName() { return name; }
    float getAnimationRangeStart() { return animationRangeStart; }
    float getAnimationRangeEnd() { return animationRangeEnd; }
    std::vector<Channel*> &getChannels() { return channels; }
    std::vector<float> &getFreedomDegrees() { return freedomDegrees; }

protected:
    std::string name;

    float animationRangeStart;
    float animationRangeEnd;
    std::vector<Channel*> channels;
    std::vector<float> freedomDegrees;
};

#endif