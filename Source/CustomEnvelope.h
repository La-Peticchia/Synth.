/*
  ==============================================================================

    CustomEnvelope.h
    Created: 5 May 2024 11:47:06am
    Author:  Slend

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Enums.h"
#include "Defines.h"

struct Ramp {
public:
    float targetValue, duration;
    Ramp(float tVal, float dur) {
        targetValue = tVal;
        duration = dur;
    }
private:

};

class CustomEnvelope {
public:

    CustomEnvelope() {

    }

    void Attack() {
        rampCount = 0;

        if (currentState == release) {

            float prevTargetValue = 0;
            Ramp maxRamp = Ramp(0, 0);
            for (int i = 0; i < attacks.size(); i++)
            {
                float currentVal = envValue.getCurrentValue(), targetValue = attacks[i]->targetValue, duration = attacks[i]->duration;

                if (currentVal <= targetValue)
                {
                    float tmp = (targetValue - currentVal) / (targetValue - prevTargetValue);
                    envValue.setCurrentAndTargetValue(currentVal);
                    envValue.reset(sampleRate, duration * tmp);
                    envValue.setTargetValue(targetValue);
                    break;
                }

                if (maxRamp.targetValue < targetValue) {
                    maxRamp.targetValue = targetValue;
                    maxRamp.duration = duration;
                }

                if (i == attacks.size() - 1) {

                    envValue.setCurrentAndTargetValue(currentVal);
                    envValue.reset(sampleRate, maxRamp.duration);
                    envValue.setTargetValue(maxRamp.targetValue);
                    break;
                }


                //if ((attacks[i]->targetValue > prevTargetValue && attacks[i]->targetValue > envValue.getCurrentValue()) || (attacks[i]->targetValue < prevTargetValue && attacks[i]->targetValue < envValue.getCurrentValue())) {
                //    
                //    envValue.setCurrentAndTargetValue(envValue.getCurrentValue());
                //    envValue.reset(sampleRate, attacks[i]->duration);
                //    envValue.setTargetValue(attacks[i]->targetValue);
                //    break;
                //}

                rampCount++;
                prevTargetValue = attacks[i]->targetValue;
            }


        }
        else
        {
            envValue.reset(sampleRate, attacks[0]->duration);
            envValue.setTargetValue(attacks[0]->targetValue);
            rampCount++;
        }

        currentRamps.clear(false);
        currentRamps.addArray(attacks);
        currentState = attack;
    }


    void Release() {

        rampCount = 0;

        if (currentState == attack) {
            float prevTargetValue = attacks[attacks.size()-1]->targetValue;
            Ramp maxRamp = Ramp(0, 0);

            for (int i = 0; i < releases.size(); i++)
            {

                float currentVal = envValue.getCurrentValue(), targetValue = releases[i]->targetValue, duration = releases[i]->duration;

                if ((currentVal >= prevTargetValue && currentVal <= targetValue) || (currentVal >= targetValue && currentVal <= prevTargetValue))
                {
                    float tmp = (targetValue - currentVal) / (targetValue - prevTargetValue);
                    envValue.setCurrentAndTargetValue(currentVal);
                    envValue.reset(sampleRate, duration * tmp);
                    envValue.setTargetValue(targetValue);
                    break;
                }

                if (maxRamp.targetValue < targetValue) {
                    maxRamp.targetValue = targetValue;
                    maxRamp.duration = duration;
                }

                if (i == attacks.size() - 1) {

                    envValue.setCurrentAndTargetValue(currentVal);
                    envValue.reset(sampleRate, maxRamp.duration);
                    envValue.setTargetValue(maxRamp.targetValue);
                    break;
                }

                //if ((releases[i]->targetValue > prevTargetValue && releases[i]->targetValue > envValue.getCurrentValue()) || (releases[i]->targetValue < prevTargetValue && releases[i]->targetValue < envValue.getCurrentValue())) {
                //    envValue.setCurrentAndTargetValue(envValue.getCurrentValue());
                //    envValue.reset(sampleRate, releases[i]->duration);
                //    envValue.setTargetValue(releases[i]->targetValue);
                //    break;
                //}

                rampCount++;
                prevTargetValue = releases[i]->targetValue;
            }
        }
        else
        {
            envValue.reset(sampleRate, releases[0]->duration);
            envValue.setTargetValue(releases[0]->targetValue);
            rampCount++;
        }

        currentRamps.clear(false);
        currentRamps.addArray(releases);
        currentState = release;

    }

    double GetNextValue() {
        float nextVal = envValue.getNextValue();

        if (juce::approximatelyEqual(std::fabs(envValue.getTargetValue() - envValue.getCurrentValue()), 0.f)) {
            if (rampCount == currentRamps.size()) {
                switch (currentState)
                {
                case attack:
                    currentState = sustain;
                    break;
                case release:
                    currentState = idle;
                    break;
                }

                return nextVal;
            }
            envValue.reset(sampleRate,currentRamps[rampCount]->duration);
            envValue.setTargetValue(currentRamps[rampCount++]->targetValue);
        }
        return nextVal;

    }

    void ResetToZero(double sampleRate) {
        this->sampleRate = sampleRate;
        envValue.setCurrentAndTargetValue(0.f);
        envValue.reset(sampleRate, rampDuration);
    }

    float GetCurrentValue() {
        return envValue.getCurrentValue();
        
    }

    void SetEnvDuration(float totalEnvDuration) {
        rampDuration = totalEnvDuration / attacks.size();
        envValue.reset(sampleRate, rampDuration);
    }

    EnvState GetCurrentState() {
        
        return currentState;
    }
    
    juce::OwnedArray<Ramp> attacks;
    juce::OwnedArray<Ramp> releases;



private:

    EnvState currentState = idle;
    juce::LinearSmoothedValue<float> envValue{ 0.0f };
    juce::OwnedArray<Ramp> currentRamps;
    float sampleRate = DEFAULT_SAMPLE_RATE;
    int rampCount = 0;
    float rampDuration = DEFAULT_RAMP_DURATION;
};