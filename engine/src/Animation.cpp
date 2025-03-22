#include "Animation.h"

#include "math/Vector.h"
#include "math/Matrix.h"

#include "core.h"
#include "utility/Tokenizer.h"

#include <cassert>

Animation::Animation()
{
}

Animation::~Animation()
{
}

void printAnimation(Animation *animation)
{
    Logger::debug("Animation name: %s\n", animation->getName().c_str());
    Logger::debug("Animation range start: %f\n", animation->getAnimationRangeStart());
    Logger::debug("Animation range end: %f\n", animation->getAnimationRangeEnd());

    for (unsigned int i = 0; i < animation->getChannels().size(); i++)
    {

        Channel channel = *animation->getChannels()[i];
        Logger::debug("Channel %d (%d):\n", i, channel.keyframes.size());

        for (unsigned int j = 0; j < channel.keyframes.size(); j++)
        {
            Keyframe keyframe = channel.keyframes[j];
            Logger::debug("Keyframe %d: time: %f, value: %f\n", j, keyframe.time, keyframe.value);
        }
    }
}

bool Animation::load(const char *filename)
{
    Logger::info("Loading animation from file: %s\n", filename);

    Tokenizer tokenizer;

    if (!tokenizer.Open(filename))
    {
        Logger::error("Failed to open file: %s\n", filename);
        return false;
    }

    ///////////////
    // ANIMATION //
    ///////////////

    if (!tokenizer.FindToken("animation"))
    {
        Logger::error("Failed to find token \"animation\" in animation file: %s\n", filename);
        return false;
    }

    tokenizer.FindToken("{");

    if (!tokenizer.FindToken("range"))
    {
        Logger::error("Failed to find token \"range\" in animation file: %s\n", filename);
        return false;
    }

    animationRangeStart = tokenizer.GetFloat();
    animationRangeEnd = tokenizer.GetFloat();

    if (!tokenizer.FindToken("numchannels"))
    {
        Logger::error("Failed to find token \"numchannels\" in animation file: %s\n", filename);
        return false;
    }

    unsigned int numChannels = (unsigned int)tokenizer.GetInt();

    Logger::debug("Number of channels: %d\n", numChannels);

    //////////////
    // CHANNELS //
    //////////////

    channels.resize(numChannels);
    freedomDegrees.resize(numChannels);

    for (unsigned int i = 0; i < numChannels; i++)
    {
        if (!tokenizer.FindToken("channel"))
        {
            Logger::error("Failed to find token \"channel\" (line %d) in animation file: %s\n", tokenizer.GetLineNum(), filename);
            return false;
        }

        const unsigned int channelNumber = (unsigned int)tokenizer.GetInt();

        Logger::debug("Channel %d\n", channelNumber);

        assert(channelNumber == i);

        tokenizer.FindToken("{");

        /////////////
        // CHANNEL //
        /////////////

        Channel &channel = *new Channel();
        channels[i] = &channel;

        if (!tokenizer.FindToken("extrapolate"))
        {
            Logger::error("Failed to find token \"extrapolate\" (line %d) in animation file: %s\n", tokenizer.GetLineNum(), filename);
            return false;
        }

        char token[256];

        tokenizer.GetToken(token);

        if (strcmp(token, "linear") == 0)
        {
            channel.extrapolationInType = ExtrapolationType::LINEAR;
        }
        else if (strcmp(token, "cycle") == 0)
        {
            channel.extrapolationInType = ExtrapolationType::CYCLE;
        }
        else if (strcmp(token, "cycle_offset") == 0)
        {
            channel.extrapolationInType = ExtrapolationType::CYCLE_OFFSET;
        }
        else if (strcmp(token, "bounce") == 0)
        {
            channel.extrapolationInType = ExtrapolationType::BOUNCE;
        }

        tokenizer.GetToken(token);

        if (strcmp(token, "linear") == 0)
        {
            channel.extrapolationOutType = ExtrapolationType::LINEAR;
        }
        else if (strcmp(token, "cycle") == 0)
        {
            channel.extrapolationOutType = ExtrapolationType::CYCLE;
        }
        else if (strcmp(token, "cycle_offset") == 0)
        {
            channel.extrapolationOutType = ExtrapolationType::CYCLE_OFFSET;
        }
        else if (strcmp(token, "bounce") == 0)
        {
            channel.extrapolationOutType = ExtrapolationType::BOUNCE;
        }

        if (!tokenizer.FindToken("keys"))
        {
            Logger::error("Failed to find token \"keys\" (line %d) in animation file: %s\n", tokenizer.GetLineNum(), filename);
            return false;
        }

        unsigned int numKeyframes = (unsigned int)tokenizer.GetInt();

        Logger::debug("Number of keyframes: %d\n", numKeyframes);

        channel.keyframes.resize(numKeyframes);

        tokenizer.FindToken("{");

        for (unsigned int j = 0; j < numKeyframes; j++)
        {
            Keyframe &keyframe = channel.keyframes[j];

            keyframe.time = tokenizer.GetFloat();
            keyframe.value = tokenizer.GetFloat();

            tokenizer.GetToken(token);

            if (strcmp(token, "flat") == 0)
            {
                keyframe.tangentInType = InterpolationType::FLAT;
            }
            else if (strcmp(token, "linear") == 0)
            {
                keyframe.tangentInType = InterpolationType::LINEAR;
            }
            else if (strcmp(token, "smooth") == 0)
            {
                keyframe.tangentInType = InterpolationType::SMOOTH;
            }
            else
            {
                keyframe.tangentInType = InterpolationType::CUSTOM_SLOPE;
                keyframe.tangentIn = atof(token);
            }

            tokenizer.GetToken(token);

            if (strcmp(token, "flat") == 0)
            {
                keyframe.tangentOutType = InterpolationType::FLAT;
            }
            else if (strcmp(token, "linear") == 0)
            {
                keyframe.tangentOutType = InterpolationType::LINEAR;
            }
            else if (strcmp(token, "smooth") == 0)
            {
                keyframe.tangentOutType = InterpolationType::SMOOTH;
            }
            else
            {
                keyframe.tangentOutType = InterpolationType::CUSTOM_SLOPE;
                keyframe.tangentOut = atof(token);
            }
        }

        tokenizer.FindToken("}");
        tokenizer.FindToken("}");
    }

    tokenizer.FindToken("}");

    tokenizer.Close();

    // DEBUG: PRINT ANIMATION
    printAnimation(this);

    Logger::info("Loaded animation from file: %s\n", filename);

    return true;
}

float evaluateKeyframe(const Channel channel, float t, float offset)
{
    ///////////////////////////////////////////////////////////////
    // 1 KEYFRAME EDGE CASE                                      //
    // IN THE 1 KEYFRAME CASE, THE EXTRAPOLATION TYPE IS IGNORED //
    ///////////////////////////////////////////////////////////////

    if (channel.keyframes.size() == 1)
    {
        return channel.keyframes[0].value;
    }

    ///////////////////
    // EXTRAPOLATION //
    ///////////////////

    if (t <= channel.keyframes[0].time)
    {
        if (channel.extrapolationInType == ExtrapolationType::CONSTANT)
        {
            return channel.keyframes[0].value;
        }
        else if (channel.extrapolationInType == ExtrapolationType::LINEAR)
        {
            // USE TANGENT OF FIRST KEYFRAME
            return channel.keyframes[0].value - (t - channel.keyframes[0].time) * channel.keyframes[0].tangentIn;
        }
        else if (channel.extrapolationInType == ExtrapolationType::CYCLE)
        {
            // SUBTRACT ONE CYCLE FROM WALL TIME AND EVALUATE
            return evaluateKeyframe(channel, t + (channel.keyframes[channel.keyframes.size() - 1].time - channel.keyframes[0].time), 0.0f);
        }
        else if (channel.extrapolationInType == ExtrapolationType::CYCLE_OFFSET)
        {
            // SUBTRACT ONE CYCLE FROM WALL TIME AND EVALUATE PLUS OFFSET
            return evaluateKeyframe(channel,
                                    t + (channel.keyframes[channel.keyframes.size() - 1].time - channel.keyframes[0].time),
                                    channel.keyframes[0].value - channel.keyframes[channel.keyframes.size() - 1].value);
        }
        else if (channel.extrapolationInType == ExtrapolationType::BOUNCE)
        {
            const float channelTimeDifference = channel.keyframes[channel.keyframes.size() - 1].time - channel.keyframes[0].time;
            const float timeDifference = channel.keyframes[0].time - t;

            return evaluateKeyframe(channel,
                                    (unsigned int)floor(timeDifference / channelTimeDifference) % 2 == 0
                                        ? fmod(timeDifference, channelTimeDifference) + channel.keyframes[0].time
                                        : channel.keyframes[1].time - fmod(timeDifference, channelTimeDifference),
                                    0.0f);
        }
        else
        {
            Logger::error("Invalid extrapolation type\n");
        }
    }
    else if (t >= channel.keyframes[channel.keyframes.size() - 1].time)
    {
        if (channel.extrapolationOutType == ExtrapolationType::CONSTANT)
        {
            return channel.keyframes[channel.keyframes.size() - 1].value;
        }
        else if (channel.extrapolationOutType == ExtrapolationType::LINEAR)
        {
            // USE TANGENT OF LAST KEYFRAME
            return channel.keyframes[channel.keyframes.size() - 1].value + (t - channel.keyframes[channel.keyframes.size() - 1].time) * channel.keyframes[channel.keyframes.size() - 1].tangentOut;
        }
        else if (channel.extrapolationOutType == ExtrapolationType::CYCLE)
        {
            // ADD ONE CYCLE TO WALL TIME AND EVALUATE
            return evaluateKeyframe(channel, t - (channel.keyframes[channel.keyframes.size() - 1].time - channel.keyframes[0].time), 0.0f);
        }
        else if (channel.extrapolationOutType == ExtrapolationType::CYCLE_OFFSET)
        {
            // ADD ONE CYCLE TO WALL TIME AND EVALUATE PLUS OFFSET
            return evaluateKeyframe(channel,
                                    t - (channel.keyframes[channel.keyframes.size() - 1].time - channel.keyframes[0].time),
                                    offset + channel.keyframes[channel.keyframes.size() - 1].value - channel.keyframes[0].value);
        }
        else if (channel.extrapolationOutType == ExtrapolationType::BOUNCE)
        {
            const float channelTimeDifference = channel.keyframes[channel.keyframes.size() - 1].time - channel.keyframes[0].time;
            const float timeDifference = t - channel.keyframes[channel.keyframes.size() - 1].time;

            return evaluateKeyframe(channel,
                                    (unsigned int)floor(timeDifference / channelTimeDifference) % 2 == 0
                                        ? channel.keyframes[1].time - fmod(timeDifference, channelTimeDifference)
                                        : fmod(timeDifference, channelTimeDifference) + channel.keyframes[0].time,
                                    0.0f);
        }
        else
        {
            Logger::error("Invalid extrapolation type\n");
        }
    }

    ///////////////////////
    // EVALUATE KEYFRAME //
    ///////////////////////

    else
    {
        ///////////////////
        // BINARY SEARCH //
        ///////////////////

        unsigned int low = 0;
        unsigned int high = static_cast<unsigned int>(channel.keyframes.size() - 1);

        while (high > low + 1)
        {
            unsigned int mid = (low + high) / 2;
            float midTime = channel.keyframes[mid].time;

            if (t < midTime)
            {
                high = mid;

                if (channel.keyframes[low].time == t)
                {
                    high = low;
                    break;
                }
            }
            else
            {
                low = mid;

                if (channel.keyframes[high].time == t)
                {
                    low = high;
                    break;
                }
            }
        }

        ///////////////////
        // INTERPOLATION //
        ///////////////////

        if (low == high)
            return channel.keyframes[low].value;

        const float valueDifference = channel.keyframes[high].value - channel.keyframes[low].value;
        const float timeDifference = channel.keyframes[high].time - channel.keyframes[low].time;

        ////////////////////////
        // CALCULATE TANGENTS //
        ////////////////////////

        float v0;
        float v1;

        if (channel.keyframes[low].tangentOutType == InterpolationType::FLAT)
        {
            v0 = 0.0f;
        }
        else if (channel.keyframes[low].tangentOutType == InterpolationType::LINEAR)
        {
            v0 = valueDifference / timeDifference;
        }
        else if (channel.keyframes[low].tangentOutType == InterpolationType::SMOOTH)
        {
            if (low == 0 || low == channel.keyframes.size() - 1)
            {
                v0 = valueDifference / timeDifference;
            }
            else
            {
                v0 = (channel.keyframes[low + 1].value - channel.keyframes[low - 1].value) / (channel.keyframes[low + 1].time - channel.keyframes[low - 1].time);
            }
        }
        else if (channel.keyframes[low].tangentOutType == InterpolationType::CUSTOM_SLOPE)
        {
            v0 = channel.keyframes[low].tangentOut;
        }
        else
        {
            Logger::error("Invalid tangent out type\n");
        }

        if (channel.keyframes[high].tangentInType == InterpolationType::FLAT)
        {
            v1 = 0.0f;
        }
        else if (channel.keyframes[high].tangentInType == InterpolationType::LINEAR)
        {
            v1 = valueDifference / timeDifference;
        }
        else if (channel.keyframes[high].tangentInType == InterpolationType::SMOOTH)
        {
            if (high == 0 || high == channel.keyframes.size() - 1)
            {
                v1 = valueDifference / timeDifference;
            }
            else
            {
                v1 = (channel.keyframes[high + 1].value - channel.keyframes[high - 1].value) / (channel.keyframes[high + 1].time - channel.keyframes[high - 1].time);
            }
        }
        else if (channel.keyframes[high].tangentInType == InterpolationType::CUSTOM_SLOPE)
        {
            v1 = channel.keyframes[high].tangentIn;
        }
        else
        {
            Logger::error("Invalid tangent in type\n");
        }

        Vector4f input = Vector4f(channel.keyframes[low].value, channel.keyframes[high].value, timeDifference * v0, timeDifference * v1);
        Matrix4x4f hermiteBasis = Matrix4x4f(2.0f, -2.0f, 1.0f, 1.0f,
                                             -3.0f, 3.0f, -2.0f, -1.0f,
                                             0.0f, 0.0f, 1.0f, 0.0f,
                                             1.0f, 0.0f, 0.0f, 0.0f);

        Vector4f output = hermiteBasis * input;

        float timeRatio = (t - channel.keyframes[low].time) / timeDifference;

        float value = output[0] * pow(timeRatio, 3) + output[1] * pow(timeRatio, 2) + output[2] * timeRatio + output[3];

        return value + offset;
    }
}

void Animation::evaluate(float wallTime)
{
    for (unsigned int i = 0; i < channels.size(); i++)
        freedomDegrees[i] = evaluateKeyframe((Channel &)*channels[i], wallTime, 0.0f);
}