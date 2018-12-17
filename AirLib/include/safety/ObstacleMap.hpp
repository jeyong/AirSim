// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef air_ObstacleMap_hpp
#define air_ObstacleMap_hpp

#include <mutex>
#include "common/Common.hpp"

namespace msr { namespace airlib {

/*
    ObstacleMap 클래스는 비행체 주변에 원형 디스크 형태로 장애물의 2D 지도를 구현.
    주요 설계 기준은 insert/delete/queries를 매우 빠르게 할 수 있도록 하는 것.
    grid 기반 접근 방법에서는 불가능(큰 상수각 가지게 되니까)
    현재 code는 O(1)로 설계. 이렇게 하면 laser 같은 센서가 빠르게 처리할 수 있도록 있게됨.
    ObstacleMap implements 2D map of obstacles in circular disk around the vehicle. The main
    design criteria is to make insert/delete/queries very fast. This is typically
    not possible in grid based approach because these operations may have large 
    constant. Current code is designed to be O(1) with small constant. This will
    enable sensors like lasers which needs much faster processing.

    circle을 가지고 이를 tick의 갯수로 나누기. ticks=4인 경우 circle은 아래와 같이 형태처럼 보임.
    We take circle and divide it in to number of ticks. If ticks=4 then circle looks
    like below with ticks marked as 0, 1, 2, 3, 4:

       0XXX1
      XX   XX
      XX   XX
       3XXX2

    핵심 부분은 tick 0은 항상 12시 방향의 왼쪽에 첫번째 tick이 있다.
    2개 tick 사이에 있는 각 segment는 circle에서 cone 형태를 가짐. 여기에 장애물 정보를 넣는다.
    tick 0-1사이에 segment는 0이고 1-2 사이에는 1과 같다. 따라서 이 방식을 이용하면 segment 0이 항상 frone cone이 된다.
    DJI Matrice Guidance 시스템에서와 같이 만약에 4개 센서만 있다면 seg 1은 오른쪽에 있는 장애물의 정보를 seg 2는 뒤에 있는 정보를 가진다. 
    The essential part is that tick 0 is always the first tick on left of 12 o'clock.
    Each segment between two ticks forms a cone in the circle where we will put the obstacle
    information. The segment between ticks 0-1 is 0, 1-2 is 1 and so on. So this scheme allows
    us to get the front cone always at segment 0. If we have only 4 sensors like in DJI Matrice
    Guidance system then segment 1 has information about obstacles on right, seg 2 for back and 
    so on.

    다른 설계 원칙은 이 class는 thread safe하다.
    하나의 thread가 연속으로 장애물 정보를 update한다. 다른 thread는 map에 대한 query를 수행.
    Another design criteria is that this class is thread safe for concurrent updates and queries.
    We fully expect one thread to continuously update the obstacles while another to query the map.
*/

class ObstacleMap {
private:
    // 각 tick segment에 대한 distance를 저장
    //stores distances for each tick segment
    vector<float> distances_;
    // 신뢰도(confidence)
    //what is the confidence in these values? This should typically be the standard deviation
    vector<float> confidences_; 
    // tick의 수 (resolution)
    //number of ticks, this decides reolution
    int ticks_;
    //blind spot에 대해서는 이웃으로부터 값을 얻는다.
    //blind spots don't get updated so we get its value from neighbours
    vector<bool> blindspots_;
public:
    // query의 결과를 반환
    //this will be return result of the queries
    struct ObstacleInfo {
        int tick;   //at what tick we found obstacle 어느 tick에서 장애물을 발견했는지
        float distance; //what is the distance from obstacle  장애물로부터의 거리
        float confidence;  

        string toString() const
        {
            return Utils::stringf("Obs: tick=%i, distance=%f, confidence=%f", tick, distance, confidence);
        }
    };

    //private version of hasObstacle doesn't do lock or check inputs
    ObstacleInfo hasObstacle_(int from_tick, int to_tick) const;    
private:
    int wrap(int tick) const;

    //currently we employ simple thread safe model: just serialize queries and updates
    std::mutex mutex_;
public:
    // odd_blindspots가 true이면 모든 odd tick을 blind spot으로 설정
    //if odd_blindspots = true then set all odd ticks as blind spots
    ObstacleMap(int ticks, bool odd_blindspots = false);

    // +/-windows tick 내에서 tick 방향에 대한 map을 업데이트
    //update the map for tick direction within +/-window ticks
    void update(float distance, int tick, int window, float confidence);
    void update(float distances[], float confidences[]);

    void setBlindspot(int tick, bool blindspot);

    // seg에서 장애물에 있는지를 query.
    //query if we have obstacle in segment that starts at from to segment that starts at to
    ObstacleInfo hasObstacle(int from_tick, int to_tick);

    // 최소 거리에 잇는 장애물을 찾기 위해서 전체 map을 검색
    //search entire map to find obstacle at minimum distance
    ObstacleInfo getClosestObstacle();

    // map이 초기화된 tick의 수
    //number of ticks the map was initialized with
    int getTicks() const;
    // radians angle(body frame에서)을 tick number로 변환
    //convert angle (in body frame) in radians to tick number
    int angleToTick(float angle_rad) const;
    // tick을 angle의 시작으로 변환
    //convert tick to start of angle (in body frame) in radians
    float tickToAngleStart(int tick) const;
    // tick을 angle의 끝으로 변환
    //convert tick to end of angle (in body frame) in radians
    float tickToAngleEnd(int tick) const;
    // tick을 cone의 중간 각으로 변환
    //convert tick to mid of the cone in radians
    float tickToAngleMid(int tick) const;    
};
    
}} //namespace
#endif
