//
// Created by chang on 11/7/25.
//

#ifndef SPEECH_D1_ARM_SPEECH_CONTROL_HPP
#define SPEECH_D1_ARM_SPEECH_CONTROL_HPP

#include <iostream>
#include <string>
#include <unitree/robot/channel/channel_publisher.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>
#include <PubServoInfo_.hpp>
#include <unitree/common/time/time_tool.hpp>
#include "ArmString_.hpp"

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
    void hold_joint();
    void zero_joint();
    void move_single();
    void move_all();
    void process_command(const std::string& command);
    void angle_handler(const void* msg);

    unitree_arm::msg::dds_::PubServoInfo_* feedback_msg_;
    std::unordered_map<std::string, int> nameToIndex_;
private:

    std::unique_ptr<ChannelPublisher<unitree_arm::msg::dds_::ArmString_>> publisher_;
    std::unique_ptr<ChannelSubscriber<unitree_arm::msg::dds_::PubServoInfo_>> subscriber_;

};


#endif //SPEECH_D1_ARM_SPEECH_CONTROL_HPP