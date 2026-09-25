cmake_minimum_required(VERSION 3.24)

set(CMAKE_CUDA_ARCHITECTURES native)
project(CFDSolver LANGUAGES CXX CUDA)

set(SFML_STATIC_LIBRARIES TRUE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)
add_executable(CFDSolver
    src/main.cpp
    src/mesh/Grid.cpp
    src/rendering/ScalarFieldRendering.cpp
    src/rendering/VelocityRendering.cpp
    src/simulation/Simulation.cpp
    src/simulation/applyExternalForces.cpp
    src/simulation/applyAdvection.cpp
    src/simulation/computeDivergence.cpp
    src/simulation/applyJacobiIteration.cpp
    src/simulation/applyPressureGradient.cpp
    src/simulation/applyBoundaries.cpp
    src/simulation/applyDye.cpp
    src/simulation/updateCellType.cpp
    src/airfoil/Airfoil.cpp
    src/cuda/jacobiPressureSolve/runJacobiPressureSolve.cu
    src/cuda/jacobiPressureSolve/applyPressureJacobiIteration.cu
    src/cuda/jacobiPressureSolve/applyPressureBoundaries.cu
    src/cuda/velocityAdvection/applyVelocityAdvection.cu
    src/cuda/velocityAdvection/runVelocityAdvection.cu
)

target_include_directories(CFDSolver PRIVATE include)
target_compile_options(CFDSolver PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-lineinfo>)

target_link_libraries(CFDSolver PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
