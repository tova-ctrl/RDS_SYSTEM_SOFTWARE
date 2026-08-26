cmake_minimum_required(VERSION 3.16)
project(ISM_Firmware CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# ── ISM core library ──────────────────────────────────────────────────────
add_library(ism_core
    src/ISM.cpp
    src/FccStub.cpp
    src/IsmEmgLink.cpp
    src/platform_linux.cpp
)
target_include_directories(ism_core PUBLIC include)
target_link_libraries(ism_core PUBLIC pthread)

# ── ISM Simulator executable (Linux/WSL — SocketCAN + EMG UDP) ───────────
add_executable(ism_sim
    src/main_sim.cpp
    src/IsmSimCan.cpp
    src/IsmSimEth.cpp
)
target_link_libraries(ism_sim PRIVATE ism_core)

# ── Fault injection test executable ──────────────────────────────────────
add_executable(fault_injection_tests
    tests/FaultInjectionTests.cpp
)
target_link_libraries(fault_injection_tests PRIVATE ism_core)

# ── Default build target message ─────────────────────────────────────────
message(STATUS "Targets: ism_sim  fault_injection_tests")
