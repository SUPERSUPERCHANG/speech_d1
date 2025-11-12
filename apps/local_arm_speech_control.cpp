//
// Created by chang on 11/7/25.
//

#include "../include/arm_control/arm_speech_control.hpp"

int main(int argc, char* argv[])
{
    ArmSpeechControl armSpeechControl;
    armSpeechControl.init();

    // 检查是否提供了启动参数
    if (argc > 1)
    {
        std::string startupCommand = argv[1]; // 从命令行参数获取第一个参数
        std::cout << "Command：" << startupCommand << std::endl;
        armSpeechControl.process_command(startupCommand);
    }
    else
    {
        std::cout << "No command in params, start intereactive mode" << std::endl;
    }

    // 进入循环交互模式
    std::string command;
    std::cout << "Input commands（open/close/hold/zero/handle/exit）：" << std::endl;

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit" || command == "quit")
        {
            std::cout << "exit..." << std::endl;
            break;
        }

        armSpeechControl.process_command(command);
    }

    return 0;
}