cmake_minimum_required(VERSION 3.10)
project(d1_arm_speech_control CXX)

# 统一 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============ 依赖与公共设置 ============
set(THIRD_PARTY_INCLUDES
        /usr/local/include/ddscxx
        /usr/local/include/iceoryx/v2.0.2
        ${PROJECT_SOURCE_DIR}/3rdparty/include
        ${PROJECT_SOURCE_DIR}/include
)

set(THIRD_PARTY_LIBS
        unitree_sdk2
        ddscxx
        ddsc
        pthread
        rt
)

# ============ 库：arm_speech_control ============
add_library(arm_speech_control STATIC)
target_sources(arm_speech_control PRIVATE
        src/arm_control/arm_speech_control.cpp
        src/msg/PubServoInfo_.cpp
        src/msg/ArmString_.cpp
)
target_include_directories(arm_speech_control
        PUBLIC ${THIRD_PARTY_INCLUDES}
)
target_link_libraries(arm_speech_control
        PUBLIC ${THIRD_PARTY_LIBS}
)

# ============ 库：tcp_socket ============
add_library(tcp_socket STATIC)
target_sources(tcp_socket PRIVATE
        src/tcp/tcp_socket.cpp
)
target_include_directories(tcp_socket
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# ============ 库：arm_fsm ============
add_library(arm_fsm STATIC)
target_sources(arm_fsm PRIVATE
        src/fsm/arm_fsm.cpp
)
target_include_directories(arm_fsm
        PUBLIC ${PROJECT_SOURCE_DIR}/include ${THIRD_PARTY_INCLUDES}
)

# ============ 可执行文件 ============
# 1) 本地测试：仅用 arm_speech_control
add_executable(local_arm_speech_control apps/local_arm_speech_control.cpp)
target_link_libraries(local_arm_speech_control PRIVATE arm_speech_control)

# 2) TCP 服务器
add_executable(test_tcp_server apps/test_tcp_server.cpp)
target_link_libraries(test_tcp_server PRIVATE tcp_socket )

# 3) TCP 客户端
add_executable(test_tcp_client apps/test_tcp_client.cpp)
target_link_libraries(test_tcp_client PRIVATE tcp_socket)

# 4) 综合：TCP + Arm（不再重复列 .cpp，只链接库）
add_executable(tcp_arm_speech_control apps/tcp_arm_speech_control.cpp)
target_link_libraries(tcp_arm_speech_control PRIVATE tcp_socket arm_speech_control)

# 5) 综合：TCP + ArmFSM
add_executable(demo_fsm apps/demo_fsm.cpp)
target_link_libraries(demo_fsm PRIVATE tcp_socket arm_fsm arm_speech_control ${THIRD_PARTY_LIBS})
target_include_directories(demo_fsm PRIVATE ${PROJECT_SOURCE_DIR}/include ${THIRD_PARTY_INCLUDES})

