cmake_minimum_required(VERSION 3.10)
project(d1_arm_speech_control)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/local/include/ddscxx /usr/local/include/iceoryx/v2.0.2 ${PROJECT_SOURCE_DIR}/include)
link_libraries(unitree_sdk2 ddsc ddscxx rt pthread)

add_library(arm_speech_control src/arm_speech_control.cpp src/msg/PubServoInfo_.cpp src/msg/ArmString_.cpp)

target_link_libraries(arm_speech_control
        unitree_sdk2
        ddscxx
        ddsc
        pthread
        rt
)


add_executable(local_arm_speech_control src/local_arm_speech_control.cpp)
target_link_libraries(local_arm_speech_control arm_speech_control)


# Add source files
add_library(tcp_socket SHARED src/tcp_socket.cpp)

# ---- Server target ----
add_executable(test_tcp_server src/test_tcp_server.cpp)
target_link_libraries(test_tcp_server PRIVATE tcp_socket pthread)

# ---- Client target ----
add_executable(test_tcp_client src/test_tcp_client.cpp)
target_link_libraries(test_tcp_client PRIVATE tcp_socket pthread)

