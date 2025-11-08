#!/bin/bash
echo "Setup Unitree ROS2 environment with robot"

# 默认接口：enp3s0（网口）
interface="enp3s0"

# 检查命令行参数
if [ "$1" == "--use-lo" ]; then
  interface="lo"
  echo "Using loopback interface (lo)"
elif [ "$1" == "--use-enp3s0" ]; then
  interface="enp3s0"
  echo "Using network interface enp3s0"
elif [ -n "$1" ]; then
  echo "Unknown option: $1"
  echo "Usage: $0 [--use-lo | --use-enp3s0]"
  exit 1
else
  echo "No interface specified, default: enp3s0"
fi

# 载入 ROS 2 环境
# source /opt/ros/humble/setup.bash

# 设置 DDS 实现
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

# 设置 CycloneDDS 网络接口
export CYCLONEDDS_URI="<CycloneDDS><Domain><General><Interfaces>
                            <NetworkInterface name=\"${interface}\" priority=\"default\" multicast=\"default\" />
                        </Interfaces></General></Domain></CycloneDDS>"

# 载入工作区
#source "${workspaceRoot}/install/setup.bash"
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:${LD_LIBRARY_PATH}
echo "Environment setup complete for interface: ${interface}"
