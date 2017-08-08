cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

project(postgres-client)

message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )

set(EXECUTABLE_NAME "postgres-client")

set(CMAKE_CXX_STANDARD 14)
add_definitions(-O2)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
message(STATUS "GTKMM_VERSION              = ${GTKMM_VERSION}")

pkg_check_modules(PQXX libpqxx)
message(STATUS "PQXX_VERSION               = ${PQXX_VERSION}")

message(STATUS "All dependencies look ok")

add_executable(${EXECUTABLE_NAME}
    src/pc.cpp
    src/main_window.cpp
    src/pg_conn.cpp
    )

link_directories(${GTKMM_LIBRARY_DIRS} ${PQXX_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS} ${PQXX_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${GTKMM_LIBRARIES} ${PQXX_LIBRARIES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
