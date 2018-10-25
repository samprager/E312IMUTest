cmake_minimum_required(VERSION 2.8)

project(E312IMUTest)

# Add compiler flags for building executables (-fPIE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

aux_source_directory(./source SRC_LIST)

FILE(GLOB_RECURSE INC_LIST "include/*.h" "include/*.hpp")

# This example also requires Boost:
set(BOOST_REQUIRED_COMPONENTS
    program_options
    system
    thread
    filesystem
)
if(MSVC)
    set(BOOST_ALL_DYN_LINK "${BOOST_ALL_DYN_LINK}" CACHE BOOL "boost enable dynamic linking")
    if(BOOST_ALL_DYN_LINK)
        add_definitions(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
    else(BOOST_ALL_DYN_LINK)
        set(BOOST_REQUIRED_COMPONENTS) #empty components list for static link
    endif(BOOST_ALL_DYN_LINK)
endif(MSVC)
find_package(Boost "1.46" REQUIRED ${BOOST_REQUIRED_COMPONENTS})


find_path(RTIMULib_INCLUDE_DIR RTIMULib.h)
find_library(RTIMULib_LIBRARY NAMES libRTIMULib)

### Configure Compiler ########################################################
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${RTIMULib_INCLUDE_DIR}
    ./include
)

link_directories(${Boost_LIBRARY_DIRS})

### Make the executable #######################################################
add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST})
message("Include list:" ${INC_LIST})
message("Src list:" ${SRC_LIST})
SET(CMAKE_BUILD_TYPE "Release")
MESSAGE(STATUS "******************************************************************************")
MESSAGE(STATUS "* NOTE: When building your own app, you probably need all kinds of different  ")
MESSAGE(STATUS "* compiler flags. This is just an example, so it's unlikely these settings    ")
MESSAGE(STATUS "* exactly match what you require. Make sure to double-check compiler and     ")
MESSAGE(STATUS "* linker flags to make sure your specific requirements are included.          ")
MESSAGE(STATUS "******************************************************************************")

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${RTIMULib_LIBRARY})

### Once it's built... ########################################################
# Here, you would have commands to install your program.
# We will skip these in this example.
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
