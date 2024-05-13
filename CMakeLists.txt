cmake_minimum_required(VERSION 3.21)
project(XIA2tree VERSION 0.9.0 LANGUAGES C CXX)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.8.0")

find_package(Threads REQUIRED)
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist)

CPMAddPackage("gh:vetlewi/Histogram@1.3.0")

add_library(structopt INTERFACE)
target_include_directories(structopt
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/third_party
)

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

CPMAddPackage(
        NAME yaml-cpp
        GITHUB_REPOSITORY jbeder/yaml-cpp
        GIT_TAG yaml-cpp-0.7.0
)

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        VERSION 0.9.5
)

CPMAddPackage(
        NAME zstr
        GITHUB_REPOSITORY mateidavid/zstr
        VERSION 1.0.7
)

add_library(XIAfuncs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Format/event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Calibrator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/MemoryMap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/MTSort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Splitter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Trigger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/XIAReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RootInterface/TTreeManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/ConfigManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/ParticleRange.cpp
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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools
)

add_library(UserSort
        ${CMAKE_CURRENT_SOURCE_DIR}/src/UserSort/DynamicLibrary.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/UserSort/UserSortManager.cpp)

target_include_directories(UserSort
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/UserSort
)

target_link_libraries(UserSort PRIVATE XIAfuncs)

set_target_properties(UserSort PROPERTIES CXX_STANDARD 17)
add_library(OCL::UserSort ALIAS UserSort)

add_library(ParticleCoincidenceSort MODULE ParticleCoincidenceSort.cpp)
target_link_libraries(ParticleCoincidenceSort PUBLIC OCL::UserSort)

add_library(Co60CoincidenceSort MODULE Co60_coincidence_sort.cpp)
target_link_libraries(Co60CoincidenceSort PUBLIC OCL::UserSort)

add_library(TimingInvestigation MODULE TimingInvestigation.cpp)
target_link_libraries(TimingInvestigation PUBLIC OCL::UserSort)

set_target_properties(XIAfuncs PROPERTIES CXX_STANDARD 17)
target_link_libraries(XIAfuncs
    PUBLIC
        OCL::Histogram
        OCL::UserSort
        readerwriterqueue
        concurrentqueue
        indicators::indicators
        structopt
        ROOT::RIO
        ROOT::Tree
        yaml-cpp::yaml-cpp
        magic_enum::magic_enum
)

target_link_libraries(ParticleCoincidenceSort PRIVATE OCL::Histogram XIAfuncs UserSort zstr::zstr)
target_link_libraries(Co60CoincidenceSort PRIVATE OCL::Histogram XIAfuncs UserSort)
target_link_libraries(TimingInvestigation PRIVATE OCL::Histogram XIAfuncs UserSort)

add_executable(XIA2tree main.cpp)
set_target_properties(XIA2tree PROPERTIES CXX_STANDARD 17)
target_link_libraries(XIA2tree PRIVATE OCL::Histogram UserSort XIAfuncs readerwriterqueue concurrentqueue)
add_dependencies(XIA2tree ParticleCoincidenceSort)
