// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef airsim_core_Environment_hpp
#define airsim_core_Environment_hpp

#include "common/Common.hpp"
#include "common/UpdatableObject.hpp"
#include "common/CommonStructs.hpp"
#include "common/EarthUtils.hpp"

// 물리엔진에 F, tau, g, p, P, T를 제공
namespace msr { namespace airlib {

class Environment : public UpdatableObject {
public:
    struct State {
        //these fields must be set at initialization time
        Vector3r position; //local NED 좌표
        GeoPoint geo_point; //global 좌표

        //these fields are computed
        // 중력가속도, 공기압력, 온도, 공기밀도
        Vector3r gravity;
        real_T air_pressure;
        real_T temperature;
        real_T air_density;

        State()
        {}
        State(const Vector3r& position_val, const GeoPoint& geo_point_val)
            : position(position_val), geo_point(geo_point_val)
        {
        }
    };
public:
    Environment()
    {
        //allow default constructor with later call for initialize
    }
    Environment(const State& initial)
    {
        initialize(initial);
    }
    void initialize(const State& initial)
    {
        initial_ = initial;

        setHomeGeoPoint(initial_.geo_point);

        updateState(initial_, home_geo_point_);
    }
    
    void setHomeGeoPoint(const GeoPoint& home_geo_point)
    {
        home_geo_point_ = HomeGeoPoint(home_geo_point);
    }

    GeoPoint getHomeGeoPoint() const
    {
        return home_geo_point_.home_geo_point;
    }

    //in local NED coordinates
    void setPosition(const Vector3r& position)
    {
        current_.position = position;
    }

    const State& getInitialState() const
    {
        return initial_;
    }
    const State& getState() const
    {
        return current_;
    }
    State& getState()
    {
        return current_;
    }

    //*** Start: UpdatableState implementation ***//
    virtual void reset()
    {
        current_ = initial_;
    }

    virtual void update()
    {
        updateState(current_, home_geo_point_);
    }
    //*** End: UpdatableState implementation ***//

private:
    static void updateState(State& state, const HomeGeoPoint& home_geo_point)
    {
        //local NED와 home_geo_point를 가지고 현재 geo_point 구하기
        state.geo_point = EarthUtils::nedToGeodetic(state.position, home_geo_point);

        // https://en.wikipedia.org/wiki/Geopotential 해수면으로부터 높이 z에 있는 단위 질량의 물체가 갖는 위치 에너지 
        // geopotential관련 한글자료 : // https://m.blog.naver.com/PostView.nhn?blogId=tnehf18&logNo=220379362224&proxyReferer=https%3A%2F%2Fwww.google.com%2F

        real_T geo_pot = EarthUtils::getGeopotential(state.geo_point.altitude / 1000.0f);
        state.temperature = EarthUtils::getStandardTemperature(geo_pot); //해수면으로부터 높이 z에서의 온도
        state.air_pressure = EarthUtils::getStandardPressure(geo_pot, state.temperature); // 해수면으로부터 높이 z에서의 기압
        state.air_density = EarthUtils::getAirDensity(state.air_pressure, state.temperature); //온도와 기압으로 공기 밀도 계산 

        //TODO: avoid recalculating square roots
        // 고도에 따른 중력 
        state.gravity = Vector3r(0, 0, EarthUtils::getGravity(state.geo_point.altitude));
    }

private:
    State initial_, current_;
    HomeGeoPoint home_geo_point_;
};

}} //namespace
#endif
