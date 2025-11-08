//
// Created by chang on 11/7/25.
//

#include "arm_speech_control.hpp"

using namespace unitree::robot;
using namespace unitree::common;

// ============================
// Constructor and Destructor
// ============================
ArmSpeechControl::ArmSpeechControl()
{
    std::cout << "[ArmSpeechControl] Start." << std::endl;
    ChannelFactory::Instance()->Init(0);

    publisher_ = std::make_unique<ChannelPublisher<unitree_arm::msg::dds_::ArmString_>>("rt/arm_Command");
    publisher_->InitChannel();

    subscriber_=std::make_unique<ChannelSubscriber<unitree_arm::msg::dds_::PubServoInfo_>>("current_servo_angle");

}

ArmSpeechControl::~ArmSpeechControl()
{
    std::cout << "[ArmSpeechControl] Destructor called." << std::endl;
}

// ============================
// initialization
// ============================
void ArmSpeechControl::init()
{
    std::cout << "[ArmSpeechControl] Initialization complete." << std::endl;
    zero_joint();
}

// ============================
// gripper
// ============================
void ArmSpeechControl::open_gripper()
{
    unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = "{\"seq\":4,\"address\":1,\"funcode\":1,\"data\":{\"id\":6,\"angle\":55,\"delay_ms\":0}}";
    publisher_->Write(msg);
    std::cout << "[ArmSpeechControl] Opening gripper..." << std::endl;
}

void ArmSpeechControl::close_gripper()
{
	unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = "{\"seq\":4,\"address\":1,\"funcode\":1,\"data\":{\"id\":6,\"angle\":15,\"delay_ms\":0}}";
    publisher_->Write(msg);

    std::cout << "[ArmSpeechControl] Closing gripper..." << std::endl;
}

// ============================
// arm
// ============================
void ArmSpeechControl::handle_joint()
{
    std::cout << "[ArmSpeechControl] Handling joint..." << std::endl;

    unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = "{\"seq\":4,\"address\":1,\"funcode\":2,\"data\":{\"mode\":1,\"angle0\":0,\"angle1\":20,\"angle2\":-30,\"angle3\":0,\"angle4\":-30,\"angle5\":0,\"angle6\":0}}";
    publisher_->Write(msg);
}

void ArmSpeechControl::hold_joint()
{
    std::cout << "[ArmSpeechControl] Holding joint position..." << std::endl;
	unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = msg.data_() = "{\"seq\":4,\"address\":1,\"funcode\":2,\"data\":{\"mode\":1,\"angle0\":0,\"angle1\":-90,\"angle2\":90,\"angle3\":0,\"angle4\":0,\"angle5\":0,\"angle6\":0}}";
    publisher_->Write(msg);
}

void ArmSpeechControl::zero_joint()
{
    std::cout << "[ArmSpeechControl] Moving all joints to zero..." << std::endl;
    unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = "{\"seq\":4,\"address\":1,\"funcode\":7}";
    publisher_->Write(msg);
}

void ArmSpeechControl::move_single(){}
void ArmSpeechControl::move_all(){}

// ============================
// parse command and execute
// ============================
void ArmSpeechControl::process_command(const std::string& command)
{
    std::cout << "[ArmSpeechControl] Received command: " << command << std::endl;

    if (command == "open")
        open_gripper();
    else if (command == "close")
        close_gripper();
    else if (command == "hold")
        hold_joint();
    else if (command == "handle")
        handle_joint();
    else if (command == "zero")
        zero_joint();
    else
        std::cerr << "[ArmSpeechControl] Unknown command: " << command << std::endl;
}




