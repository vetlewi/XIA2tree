cmake_minimum_required(VERSION 3.15)
project(XIA2tree VERSION 1.4.0 LANGUAGES C CXX)

# Default to the project version when git information is unavailable
set(FULL_VERSION "${PROJECT_VERSION}")

option(ENABLE_TESTS "Build and register tests" OFF)
option(ENABLE_SANITIZERS "Enable all sanetizers on binary" OFF)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
include(cmake/CPM.cmake)
include(cmake/GitVersion.cmake)
get_version_from_git()

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.13.0")

find_package(Threads REQUIRED)
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist)

CPMAddPackage("gh:vetlewi/Histogram@2.0.0")

add_library(structopt INTERFACE)
target_include_directories(structopt
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/third_party
)

CPMAddPackage(
        NAME readerwriterqueue
        GITHUB_REPOSITORY cameron314/readerwriterqueue
        GIT_TAG 211616e0554f93152ab3108b8d93fbc23174a9d9
)

CPMAddPackage(
        NAME concurrentqueue
        GITHUB_REPOSITORY cameron314/concurrentqueue
        #VERSION 1.0.4
        GIT_TAG c68072129c8a5b4025122ca5a0c82ab14b30cb03
)

CPMAddPackage(
        NAME indicators
        GITHUB_REPOSITORY p-ranav/indicators
        #VERSION 2.3
        GIT_TAG 3872f37abd90d7557bac5f834bfb45bd6c75259a
        OPTIONS "INDICATORS_BUILD_TESTS OFF" "INDICATORS_SAMPLES OFF" "INDICATORS_DEMO OFF"
)

CPMAddPackage(
        NAME yaml-cpp
        GITHUB_REPOSITORY jbeder/yaml-cpp
        GIT_TAG 28f93bdec6387d42332220afa9558060c8016795
)

CPMAddPackage(
        NAME magic_enum
        GITHUB_REPOSITORY Neargye/magic_enum
        VERSION 0.9.6
)

#CPMAddPackage(
#        NAME zstr
#        GITHUB_REPOSITORY mateidavid/zstr
#        VERSION 1.0.7
#)

CPMAddPackage(
        NAME cmake-scripts
        GITHUB_REPOSITORY StableCoder/cmake-scripts
        GIT_TAG 003c241725972de76e87afc0986209829b9a7d4f
)

### Setup sanetizers if ON ###
if ( ENABLE_SANITIZERS )
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
    add_sanitizer_support(thread)
endif()

### Generate version file ###
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

add_library(XIAfuncs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Format/event.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Buffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/BufferSE.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Calibrator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Sort.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/MTSort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Splitter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/SplitterSE.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/Trigger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks/XIAReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/RootInterface/TTreeManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/ConfigManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/ParticleRange.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhysicalParam/XIA_CFD.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/CommandLineInterface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/MemoryMap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/ProgressUI.cpp
)
set_target_properties(XIAfuncs PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(XIAfuncs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Format
        ${CMAKE_CURRENT_SOURCE_DIR}/include/PhysicalParam
        ${CMAKE_CURRENT_SOURCE_DIR}/include/RootInterface
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Tasks
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
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

set_target_properties(UserSort PROPERTIES CXX_STANDARD 20)
set_target_properties(UserSort PROPERTIES POSITION_INDEPENDENT_CODE ON)
add_library(OCL::UserSort ALIAS UserSort)

add_library(ParticleCoincidenceSort MODULE ParticleCoincidenceSort.cpp)
set_target_properties(ParticleCoincidenceSort PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(ParticleCoincidenceSort PUBLIC OCL::UserSort)

add_library(Co60CoincidenceSort MODULE Co60_coincidence_sort.cpp)
set_target_properties(Co60CoincidenceSort PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(Co60CoincidenceSort PUBLIC OCL::UserSort)

add_library(TimingInvestigation MODULE TimingInvestigation.cpp)
set_target_properties(TimingInvestigation PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(TimingInvestigation PUBLIC OCL::UserSort)

set_target_properties(XIAfuncs PROPERTIES CXX_STANDARD 20)
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

target_link_libraries(ParticleCoincidenceSort PRIVATE OCL::Histogram XIAfuncs UserSort)
target_link_libraries(Co60CoincidenceSort PRIVATE OCL::Histogram XIAfuncs UserSort)
target_link_libraries(TimingInvestigation PRIVATE OCL::Histogram XIAfuncs UserSort)

add_executable(XIA2tree main.cpp)
set_target_properties(XIA2tree PROPERTIES CXX_STANDARD 20)
target_link_libraries(XIA2tree PRIVATE OCL::Histogram UserSort XIAfuncs readerwriterqueue concurrentqueue)
add_dependencies(XIA2tree ParticleCoincidenceSort)

add_executable(FileInspector FileInspector.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Tools/MemoryMap.cpp)
target_include_directories(FileInspector
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Format
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Tools
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
set_target_properties(FileInspector PROPERTIES CXX_STANDARD 20)


include(GNUInstallDirs)
install(TARGETS XIA2tree
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()