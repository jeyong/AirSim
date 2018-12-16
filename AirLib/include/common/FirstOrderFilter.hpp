// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#ifndef airsimcore_firstorderfilter_hpp
#define airsimcore_firstorderfilter_hpp

#include <cmath>
#include "UpdatableObject.hpp"
#include "Common.hpp"

namespace msr { namespace airlib {

template <typename T>
class FirstOrderFilter : UpdatableObject {
    /*
    1차 필터 적용을 위한 class
    가속/감속 시간 상수
    This class can be used to apply a first order filter on a signal.
    It allows different acceleration and deceleration time constants.

    1차 시스템의 비연속 시간 구현 (라플라스)
    Short review of discrete time implementation of first order system:
    Laplace:
    X(s)/U(s) = 1/(tau*s + 1)
    continuous time system:  //연속 시간 시스템
    dx(t) = (-1/tau)*x(t) + (1/tau)*u(t)
    discretized system (ZoH):  //비연속 시스템
    x(k+1) = exp(samplingTime*(-1/tau))*x(k) + (1 - exp(samplingTime*(-1/tau))) * u(k)
    */
public:
    FirstOrderFilter()
    {
        //allow default constructor with later call for initialize
    }
    FirstOrderFilter(float timeConstant, T initial_input, T initial_output)
    {
        initialize(timeConstant, initial_input, initial_output);
    }
    void initialize(float timeConstant, T initial_input, T initial_output)
    {
        timeConstant_ = timeConstant;
        initial_input_ = initial_input;
        initial_output_ = initial_output;
    }

    //*** Start: UpdatableState implementation ***//
    virtual void reset() override
    {
        UpdatableObject::reset();

        last_time_ = clock()->nowNanos();
        input_ = initial_input_;
        output_ = initial_output_;
    }

    virtual void update() override
    {
        UpdatableObject::update();

        TTimeDelta dt = clock()->updateSince(last_time_);

        //lower the weight for previous value if its been long time
        //TODO: minimize use of exp
        double alpha = exp(-dt / timeConstant_);
        // x(k+1) = Ad*x(k) + Bd*u(k)
        output_ = static_cast<real_T>(output_ * alpha + input_ * (1 - alpha));
    }
    //*** End: UpdatableState implementation ***//


    void setInput(T input)
    {
        input_ = input;
    }
    T getInput() const
    {
        return input_;
    }

    T getOutput() const
    {
        return output_;
    }

private:
    float timeConstant_;
    T output_, input_;
    T initial_output_, initial_input_;
    TTimePoint last_time_;
};

}} //namespace
#endif
