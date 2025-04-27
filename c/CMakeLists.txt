cmake_minimum_required(VERSION 3.28)
PROJECT(yivo VERSION "2023.08.27" LANGUAGES C)

set(CMAKE_C_STANDARD 23)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS ON)

if(PROJECT_IS_TOP_LEVEL)
  cmake_host_system_information(RESULT HOST QUERY HOSTNAME)
  # cmake_host_system_information(RESULT CORES QUERY NUMBER_OF_LOGICAL_CORES)
  cmake_host_system_information(RESULT OSN QUERY OS_NAME)
  cmake_host_system_information(RESULT OS_VERSION QUERY OS_RELEASE)
  cmake_host_system_information(RESULT PROC QUERY PROCESSOR_DESCRIPTION)

  message(STATUS "-------------------------------------")
  message(STATUS "  Project: ${PROJECT_NAME}")
  message(STATUS "  C ${CMAKE_C_STANDARD}")
  message(STATUS "-------------------------------------")
  message(STATUS " ${HOST}")
  message(STATUS " ${OSN}: ${OS_VERSION}")
  message(STATUS " ${PROC}")
  message(STATUS "-------------------------------------")

  set(BUILD_EXAMPLES ON)
  set(BUILD_SCRUFFY ON)
else()
  message(STATUS "-> ${PROJECT_NAME} is submodule")
  set(BUILD_EXAMPLES OFF)
  set(BUILD_SCRUFFY OFF)
endif()

add_library(${PROJECT_NAME} 
    src/yivopkt.c
    src/parser.c
)
target_include_directories(${PROJECT_NAME} PUBLIC src/)

# Examples -----------------------------------------------------------
message(STATUS "-> Building ${PROJECT_NAME} examples is ${BUILD_EXAMPLES}")
if(${BUILD_EXAMPLES})
  add_subdirectory(examples)
endif()

# Tests --------------------------------------------------------------
message(STATUS "-> Building ${PROJECT_NAME} scruffy is ${BUILD_SCRUFFY}")
if(${BUILD_SCRUFFY})
  add_subdirectory(extlibs/scruffy)
  add_subdirectory(scruffy)
endif()
