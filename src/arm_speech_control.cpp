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
    subscriber_->InitChannel([this](const void* msg){ this->angle_handler(msg); });

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
bool ArmSpeechControl::handle_joint()
{
    std::cout << "[ArmSpeechControl] Handling joint..." << std::endl;
    // {0, 20, -30, 0, -30, 0, 0}
    ArmJointAngles target{0.0, 20.0, -30.0, 0.0, -30.0, 0.0, 0.0};
    move_all(target);
}

bool ArmSpeechControl::hold_joint()
{
    std::cout << "[ArmSpeechControl] Holding joint position..." << std::endl;
    // {0, -90, 90, 0, 0, 0, 0}
    ArmJointAngles target{0.0, -90.0, 90.0, 0.0, 0.0, 0.0, 0.0};
    move_all(target);
}

bool ArmSpeechControl::zero_joint()
{
    std::cout << "[ArmSpeechControl] Moving all joints to zero..." << std::endl;
    unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = "{\"seq\":4,\"address\":1,\"funcode\":7}";
    publisher_->Write(msg);
}

void ArmSpeechControl::move_single(int id, double angleDeg)
{
if (!publisher_) {
        std::cerr << "[ArmSpeechControl] publisher_ is null, cannot Write.\n";
        return;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "{\"seq\":4,\"address\":1,\"funcode\":1,"
        << "\"data\":{\"id\":" << id
        << ",\"angle\":" << angleDeg
        << ",\"delay_ms\":0}}";

    unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = oss.str();

    publisher_->Write(msg);

    std::cout << "[ArmSpeechControl] Moving single joint id=" 
              << id << " to angle=" << angleDeg << " deg" << std::endl;

}

void ArmSpeechControl::move_all(ArmJointAngles targetAnglesDeg){


    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "{\"seq\":4,\"address\":1,\"funcode\":2,\"data\":{\"mode\":1"
        << ",\"angle0\":" << targetAnglesDeg.angle0
        << ",\"angle1\":" << targetAnglesDeg.angle1
        << ",\"angle2\":" << targetAnglesDeg.angle2
        << ",\"angle3\":" << targetAnglesDeg.angle3
        << ",\"angle4\":" << targetAnglesDeg.angle4
        << ",\"angle5\":" << targetAnglesDeg.angle5
        << ",\"angle6\":" << targetAnglesDeg.angle6
        << "}}";

 
    unitree_arm::msg::dds_::ArmString_ msg{};
    msg.data_() = oss.str();

    if (publisher_) {
        publisher_->Write(msg);
    } else {
        std::cerr << "[ArmSpeechControl] publisher_ is null, cannot Write.\n";
    }
}

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

void ArmSpeechControl::angle_handler(const void* msg)
{
    const unitree_arm::msg::dds_::PubServoInfo_* pm = (const unitree_arm::msg::dds_::PubServoInfo_*)msg;
    std::cout << "servo0_data:" << pm->servo0_data_() << ", servo1_data:" << pm->servo1_data_() << ", servo2_data:" << pm->servo2_data_()<< ", servo3_data:" << pm->servo3_data_()<< ", servo4_data:" << pm->servo4_data_()<< ", servo5_data:" << pm->servo5_data_()<< ", servo6_data:" << pm->servo6_data_() << std::endl;
    feedback_angles_.angle0=pm->servo0_data_();
    feedback_angles_.angle1=pm->servo1_data_();
    feedback_angles_.angle2=pm->servo2_data_();
    feedback_angles_.angle3=pm->servo3_data_();
    feedback_angles_.angle4=pm->servo4_data_();
    feedback_angles_.angle5=pm->servo5_data_();
    feedback_angles_.angle6=pm->servo6_data_();
}


bool ArmSpeechControl::is_move_success(const ArmJointAngles& targetAnglesDeg, double tolDeg)
{

    auto within = [&](double fbVal, double tgtVal) {
        return std::abs(fbVal - tgtVal) <= tolDeg;
    };

    bool ok0 = within(feedback_angles_.angle0, targetAnglesDeg.angle0);
    bool ok1 = within(feedback_angles_.angle1, targetAnglesDeg.angle1);
    bool ok2 = within(feedback_angles_.angle2, targetAnglesDeg.angle2);
    bool ok3 = within(feedback_angles_.angle3, targetAnglesDeg.angle3);
    bool ok4 = within(feedback_angles_.angle4, targetAnglesDeg.angle4);
    bool ok5 = within(feedback_angles_.angle5, targetAnglesDeg.angle5);
    bool ok6 = within(feedback_angles_.angle6, targetAnglesDeg.angle6);

    bool allOk = ok0 && ok1 && ok2 && ok3 && ok4 && ok5 && ok6;

    if (!allOk) {
        auto diff = [](double a, double b){ return a - b; };
        std::cout << std::fixed << std::setprecision(2)
                  << "[ArmSpeechControl] reach check (tol=" << tolDeg << "°):\n"
                  << "  j0 fb=" << feedback_angles_.angle0 << " tgt=" << targetAnglesDeg.angle0 << " diff=" << diff(feedback_angles_.angle0, targetAnglesDeg.angle0) << " -> " << (ok0?"OK":"NG") << "\n"
                  << "  j1 fb=" << feedback_angles_.angle1 << " tgt=" << targetAnglesDeg.angle1 << " diff=" << diff(feedback_angles_.angle1, targetAnglesDeg.angle1) << " -> " << (ok1?"OK":"NG") << "\n"
                  << "  j2 fb=" << feedback_angles_.angle2 << " tgt=" << targetAnglesDeg.angle2 << " diff=" << diff(feedback_angles_.angle2, targetAnglesDeg.angle2) << " -> " << (ok2?"OK":"NG") << "\n"
                  << "  j3 fb=" << feedback_angles_.angle3 << " tgt=" << targetAnglesDeg.angle3 << " diff=" << diff(feedback_angles_.angle3, targetAnglesDeg.angle3) << " -> " << (ok3?"OK":"NG") << "\n"
                  << "  j4 fb=" << feedback_angles_.angle4 << " tgt=" << targetAnglesDeg.angle4 << " diff=" << diff(feedback_angles_.angle4, targetAnglesDeg.angle4) << " -> " << (ok4?"OK":"NG") << "\n"
                  << "  j5 fb=" << feedback_angles_.angle5 << " tgt=" << targetAnglesDeg.angle5 << " diff=" << diff(feedback_angles_.angle5, targetAnglesDeg.angle5) << " -> " << (ok5?"OK":"NG") << "\n"
                  << "  j6 fb=" << feedback_angles_.angle6 << " tgt=" << targetAnglesDeg.angle6 << " diff=" << diff(feedback_angles_.angle6, targetAnglesDeg.angle6) << " -> " << (ok6?"OK":"NG") << "\n";
    }

    return allOk;
}




