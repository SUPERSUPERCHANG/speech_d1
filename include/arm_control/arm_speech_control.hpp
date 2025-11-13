//
// Created by chang on 11/7/25.
//

#ifndef SPEECH_D1_ARM_SPEECH_CONTROL_HPP
#define SPEECH_D1_ARM_SPEECH_CONTROL_HPP

#include <iostream>
#include <string>
#include <unitree/robot/channel/channel_publisher.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>
#include <msg/PubServoInfo_.hpp>
#include <unitree/common/time/time_tool.hpp>
#include "../msg/ArmString_.hpp"

#define GRIPPER_OPEN_ANGLE 49.5
#define GRIPPER_CLOSE_ANGLE 25.0

using namespace unitree::robot;
using namespace unitree::common;


typedef struct ArmJointAngles
{
    double angle0;
    double angle1;
    double angle2;
    double angle3;
    double angle4;
    double angle5;
    double angle6;
} ArmJointAngles;


class ArmSpeechControl
{
public:
    // constructor and destructor
    ArmSpeechControl();
    ~ArmSpeechControl();

    // initialization
    void init();

    // gripper
    void open_gripper();
    void close_gripper();
    void handle_joint();
    void release_joint();
    void hold_joint();
    void zero_joint();
    void move_single(int id, double angleDeg);
    void move_all(ArmJointAngles targetAnglesDeg);
    void process_command(const std::string& command);
    void angle_handler(const void* msg);
    bool is_move_success(const ArmJointAngles& targetAnglesDeg, double tolDeg);
    bool is_gripper_success(const ArmJointAngles& targetAnglesDeg, double tolDeg);
    ArmJointAngles feedback_angles_;
    std::unordered_map<std::string, int> nameToIndex_;

    inline static const std::unordered_map<std::string, ArmJointAngles> armTargets = {
        {"handle", {0.0, 30.0, -30.0, 0.0, -35.0, 0.0, GRIPPER_OPEN_ANGLE}},
        {"hold",   {0.0, -90.0, 90.0, 0.0,  0.0,  0.0, GRIPPER_CLOSE_ANGLE}},
        {"zero",   {0.0, 0.0,   0.0,  0.0,  0.0,  0.0, GRIPPER_CLOSE_ANGLE}},
        {"open",   {0.0, 0.0,   0.0,  0.0,  0.0,  0.0, GRIPPER_OPEN_ANGLE}},
        {"close",   {0.0, 0.0,   0.0,  0.0,  0.0,  0.0, GRIPPER_CLOSE_ANGLE}},
        {"release",{0.0, 30.0, -30.0, 0.0, -35.0, 0.0, GRIPPER_CLOSE_ANGLE}}
    };
private:

    std::unique_ptr<ChannelPublisher<unitree_arm::msg::dds_::ArmString_>> publisher_;
    std::unique_ptr<ChannelSubscriber<unitree_arm::msg::dds_::PubServoInfo_>> subscriber_;

};


#endif //SPEECH_D1_ARM_SPEECH_CONTROL_HPP