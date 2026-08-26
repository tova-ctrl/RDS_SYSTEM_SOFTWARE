cmake_minimum_required(VERSION 3.16)
project(FCC_App CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

# ── FCC application ───────────────────────────────────────────────────────────
add_executable(fcc_app
    src/main.cpp
    src/CanBus.cpp
    src/EthCanBus.cpp
    src/GcsLink.cpp
    src/FccController.cpp
    src/EthernetServer.cpp
    src/HttpServer.cpp
    src/GimbalControl.cpp
    src/CameraStream.cpp
    src/MotorSimulator.cpp
    src/CameraApi.cpp
    src/stkAPI.c
)

target_include_directories(fcc_app PRIVATE include)
target_link_libraries(fcc_app PRIVATE pthread)

message(STATUS "Target: fcc_app")
message(STATUS "Run on Jetson:")
message(STATUS "  sudo ip link set can0 type can bitrate 1000000")
message(STATUS "  sudo ip link set can0 up")
message(STATUS "  ./fcc_app can0 <gcs_ip> 5200")
