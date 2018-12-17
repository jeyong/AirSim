// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef air_SafetyEval_hpp
#define air_SafetyEval_hpp

#include <functional>
#include <array>
#include <memory>
#include "ObstacleMap.hpp"
#include "common/common_utils/Utils.hpp"
#include "IGeoFence.hpp"
#include "common/Common.hpp"
#include "vehicles/multirotor/api/MultirotorCommon.hpp"
#include "common/common_utils/EnumFlags.hpp"

namespace msr { namespace airlib {

// 이 class는 NEU 좌표로 모든 입출력을 받음
//this class takes all inputs and outputs in NEU world coordinates in metric system
class SafetyEval {
public:
    enum class SafetyViolationType_ :uint {
        NoSafetyViolation =                      0,
        GeoFence =                  1 << 0,
        Obstacle =                  1 << 1,
        VelocityLimit =             1 << 2,
        All =                       Utils::max<uint>()
    };
    //add bitwise operators for enum
    typedef common_utils::EnumFlags<SafetyViolationType_>  SafetyViolationType;

    // 전체 장애물 회피를 비활성화시키기 위해서, SafetyViolationType에서 비활성화
    //to disable obstacle avoidance entirely, disable it in SafetyViolationType
    enum class ObsAvoidanceStrategy : uint {
        RaiseException =        0,    //will return suggested velocity vector = 0
        ClosestMove,        //find closest obstacle free destination along desired destination
        OppositeMove        //find closest obstacle free destination along opposite of desired destination
    };

    // safety evaluation의 결과
    //result of the safety evaluation
    struct EvalResult {
        // is_safe가 true인 경우에는 자료구조에 일부만 정보가 채워져 있을 수 있음. 
        //information in data structure may only be partially filled if is_safe = true
        bool is_safe;
        SafetyViolationType reason;
        // 현재, 목적지 방향 주위의 장애물 정보, 장애물을 발견하지 못한 경우에는 cur는 evaluation하지 않음.
        //obstacle info around cur and towards dest, if no obs found then cur might not be evaluated
        ObstacleMap::ObstacleInfo cur_obs, dest_obs, suggested_obs;
        // evaluation하는 동안 고려한 위치
        //locations that were considered while evaluation
        Vector3r cur_pos, dest_pos;
        // body frame에서 cur를 des vector로 변환
        //transformed cur to destination vector in body frame
        Vector3r cur_dest_body;
        string message;
        // 장애물 없는 제시한 unit vector, suggestion이 유효하지 않은 경우 반드시 0이여야 한다.
        //suggested unit vector without obstacle, must be zero if no suggestions available
        Vector3r suggested_vec;
        // risk 거리는 risk 구역이랑 얼마나 떨어져 있는지를 표시. 작은값(<=0)이 큰값보다 낫다
        // cur는 현재 위치 주변의 risk 거리
        // desk는 목적지 방향의 risk 거리
        //risk distances indicates how far we are in to risk zone, lower (<= 0) better than higher
        //cur is for risk distance around current position
        //dest if risk distance towards destination
        float cur_risk_dist, dest_risk_dist;

        // 기본 결과를 설정
        //setup default result
        EvalResult()
            :  is_safe(true), reason(SafetyViolationType_::NoSafetyViolation), suggested_vec(Vector3r::Zero()),
                cur_risk_dist(Utils::nan<float>()), dest_risk_dist(Utils::nan<float>())
        {}

        string toString() const
        {
            return Utils::stringf("SafetyEval: is_safe=%i, reason=%X, cur_risk_dist=%f, dest_risk_dist=%f, suggested_vec=%s,"
                " cur=%s, dest=%s, cur_dest_body=%s, cur_obs=[%s], dest_obs=[%s], suggested_obs=[%s], message=%s",
                is_safe, uint(reason), cur_risk_dist, dest_risk_dist, VectorMath::toString(suggested_vec).c_str(),
                VectorMath::toString(cur_pos).c_str(), VectorMath::toString(dest_pos).c_str(), VectorMath::toString(cur_dest_body).c_str(),
                cur_obs.toString().c_str(), dest_obs.toString().c_str(), suggested_obs.toString().c_str(), message.c_str());
        }
    };

private:
    MultirotorApiParams vehicle_params_;
    shared_ptr<IGeoFence> fence_ptr_;
    shared_ptr<ObstacleMap> obs_xy_ptr_;
    SafetyViolationType enable_reasons_ = SafetyEval::SafetyViolationType_::GeoFence;
    ObsAvoidanceStrategy obs_strategy_ = SafetyEval::ObsAvoidanceStrategy::RaiseException;

    void checkFence(const Vector3r& cur_pos, const Vector3r& dest_pos, EvalResult& appendToResult);
    void isSafeDestination(const Vector3r& dest,const Vector3r& cur_pos, const Quaternionr& quaternion, SafetyEval::EvalResult& result);
    Vector3r getDestination(const Vector3r& cur_pos, const Vector3r& velocity) const;
    bool isThisRiskDistLess(float this_risk_dist, float other_risk_dist) const;
    void isCurrentSafer(SafetyEval::EvalResult& result);
    void setSuggestedVelocity(SafetyEval::EvalResult& result, const Quaternionr& quaternion);
    float adjustClearanceForPrStl(float base_clearance, float obs_confidence);
public:
    SafetyEval(MultirotorApiParams vehicle_params, shared_ptr<IGeoFence> fence_ptr, shared_ptr<ObstacleMap> obs_xy);
    EvalResult isSafeVelocity(const Vector3r& cur_pos, const Vector3r& velocity, const Quaternionr& quaternion);
    EvalResult isSafeVelocityZ(const Vector3r& cur_pos, float vx, float vy, float z, const Quaternionr& quaternion);
    EvalResult isSafeDestination(const Vector3r& dest,const Vector3r& cur_pos, const Quaternionr& quaternion);
    EvalResult isSafePosition(const Vector3r& cur_pos, const Quaternionr& quaternion);
    
    void setSafety(SafetyViolationType enable_reasons, float obs_clearance, SafetyEval::ObsAvoidanceStrategy obs_strategy,
        const Vector3r& origin, float xy_length, float max_z, float min_z);
    void setObsAvoidanceStrategy(SafetyEval::ObsAvoidanceStrategy obs_strategy);
    SafetyEval::ObsAvoidanceStrategy getObsAvoidanceStrategy();
};

}} //namespace
#endif
