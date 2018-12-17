// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef msr_airlib_SensorBase_hpp
#define msr_airlib_SensorBase_hpp

#include "common/Common.hpp"
#include "common/UpdatableObject.hpp"
#include "common/CommonStructs.hpp"
#include "physics/Environment.hpp"
#include "physics/Kinematics.hpp"


namespace msr { namespace airlib {

/*
	상속한 class는 생성자에서 다른 일을 하지 않음. 
	상속한 class의 생성자 후에 초기화(initialize)를 해야함. reset을 호출해서 센서를 사용가능한 상태로 설정.
    Derived classes should not do any work in constructor which requires ground truth.
    After construction of the derived class an initialize(...) must be made which would
    set the sensor in good-to-use state by call to reset.
*/
class SensorBase : public UpdatableObject  {
public:
	// 센서 타입
    enum class SensorType : uint {
        Barometer = 1,
        Imu = 2,
        Gps = 3,
        Magnetometer = 4,
        Distance = 5,
        Lidar = 6
    };

    SensorBase(const std::string& sensor_name = "")
        : name_(sensor_name)
    {}

protected:
	// 지상 실측 정보
    struct GroundTruth {
        const Kinematics::State* kinematics;
        const Environment* environment;
    };
public:
    virtual void initialize(const Kinematics::State* kinematics, const Environment* environment)
    {
        ground_truth_.kinematics = kinematics;
        ground_truth_.environment = environment;
    }

    const GroundTruth& getGroundTruth() const
    {
        return ground_truth_;
    }
   
    const std::string& getName() const
    {
        return name_;
    }

    virtual ~SensorBase() = default;

private:
	//ground truth는 센서들 사이에서 공유하는 정보
    //ground truth can be shared between many sensors
    GroundTruth ground_truth_;
    std::string name_ = "";
};



}} //namespace
#endif
