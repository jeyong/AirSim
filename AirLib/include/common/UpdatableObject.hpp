// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef airsim_core_UpdatableObject_hpp
#define airsim_core_UpdatableObject_hpp

#include "common/Common.hpp"
#include "StateReporter.hpp"
#include "ClockFactory.hpp"

namespace msr { namespace airlib {

/*
UpdatableObject는 "ticked"이 있는 객체에 대한 일반적인 framework.
10ms마다 position을 업데이트하는 physics object가 여기에 해당할 수 있다. 
이런 객체들은 state를 가지는데, 매 tick마다 계산을 통해 새로운 state를 가진다.
reset()은 원래 상태로 돌리는 rollback을 제공. 

객체가 생성되고 초기화된 후에, update()가 호출되기 전에 reset()이 호출되어야만 한다. 
reset()은 생성자에 초기화 단계에서 호출하지 않는다. init->reset의 순서로 호출되는 것은 올바르지 않음.

UpdatableObject provides generalized framework for things that needs to be "ticked". For example,
physics objects that needs to update its position every 10ms.
Typically this objects will take their current state, do some processing and produce new state
on every tick.
The reset() provides important ability to rollback all changes to the state back to original
since when it was first initialized. This allows to reset simulation and put all updatable objects
back to their original state.

After object is created and initialized, reset() must be called first before calling update().
Do not call reset() from constructor or initialization because that will produce sequence of
init->reset calls for base-derived class that would be incorrect.
*/


class UpdatableObject {
public:
    virtual void reset()
    {
        if (reset_called && !update_called)
            throw std::runtime_error("Multiple reset() calls detected without call to update()");

        reset_called = true;
    }
    virtual void update()
    {
        if (!reset_called)
            throw std::runtime_error("reset() must be called first before update()");
        update_called = true;
    }

    virtual ~UpdatableObject() = default;

    virtual void reportState(StateReporter& reporter)
    {
        unused(reporter);
        //default implementation doesn't do anything
    }

    virtual UpdatableObject* getPhysicsBody()
    {
        return nullptr;
    }

    virtual ClockBase* clock()
    {
        return ClockFactory::get();
    }
    virtual const ClockBase* clock() const
    {
        return ClockFactory::get();
    }

protected:
    void clearResetUpdateAsserts()
    {
        reset_called = false;
        update_called = false;
    }

private:
    bool reset_called = false;
    bool update_called = false;
};

}} //namespace
#endif
