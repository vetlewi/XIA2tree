cmake_minimum_required(VERSION 3.21)
project(XIA2tree VERSION 0.9.0 LANGUAGES C CXX)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.8.0")

find_package(Threads REQUIRED)
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist)

CPMAddPackage("gh:vetlewi/Histogram@1.0.1")

CPMAddPackage(
        NAME structopt
        GITHUB_REPOSITORY p-ranav/structopt
        GIT_TAG ffe3473c1cf961ead13eda8e2e4f6e7e79fd9c5c
        OPTIONS "STRUCTOPT_TESTS OFF" "STRUCTOPT_SAMPLES OFF")

CPMAddPackage(
        NAME readerwriterqueue
        GITHUB_REPOSITORY cameron314/readerwriterqueue
        GIT_TAG 09732844aff873e8ec489e9005c775b6b1c58fa2
)

CPMAddPackage(
        NAME concurrentqueue
        GITHUB_REPOSITORY cameron314/concurrentqueue
        VERSION 1.0.3
)

CPMAddPackage(
        NAME indicators
        GITHUB_REPOSITORY p-ranav/indicators
        VERSION 2.2
        OPTIONS "INDICATORS_BUILD_TESTS OFF" "INDICATORS_SAMPLES OFF" "INDICATORS_DEMO OFF"
)

add_library(XIAfuncs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Format/event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Calibrator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/MemoryMap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Sort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Splitter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Trigger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/XIAReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/Calibration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/experimentsetup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/Parameters.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/XIA_CFD.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/CommandLineInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/ProgressUI.cpp
)

target_include_directories(XIAfuncs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Format
        ${CMAKE_CURRENT_SOURCE_DIR}/include/PhysicalParam
        ${CMAKE_CURRENT_SOURCE_DIR}/include/RootInterface
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Tasks
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools)

add_library(ParticleCoincidenceSort MODULE
        ParticleCoincidenceSort.cpp)

set_target_properties(XIAfuncs PROPERTIES CXX_STANDARD 17)
target_link_libraries(XIAfuncs PUBLIC OCL::Histogram readerwriterqueue concurrentqueue indicators::indicators structopt::structopt)

target_link_libraries(ParticleCoincidenceSort PRIVATE OCL::Histogram XIAfuncs)

add_executable(XIA2tree main.cpp)
set_target_properties(XIA2tree PROPERTIES CXX_STANDARD 17)
target_link_libraries(XIA2tree PRIVATE OCL::Histogram XIAfuncs readerwriterqueue concurrentqueue)
add_dependencies(XIA2tree ParticleCoincidenceSort)
