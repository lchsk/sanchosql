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
add_definitions(-O0)

find_package(PkgConfig)

pkg_check_modules(GTKSOURCEVIEW gtksourceviewmm-3.0)
message(STATUS "GTKSOURCEVIEWMM_VERSION    = ${GTKSOURCEVIEW_VERSION}")

pkg_check_modules(GTKMM gtkmm-3.0)
message(STATUS "GTKMM_VERSION              = ${GTKMM_VERSION}")

pkg_check_modules(PQXX libpqxx)
message(STATUS "PQXX_VERSION               = ${PQXX_VERSION}")

message(STATUS "All dependencies look ok")
message(STATUS "Generating resources...")

execute_process(
  COMMAND glib-compile-resources --target=src/resources.c --generate-source res/resources.gresource.xml
  )

add_executable(${EXECUTABLE_NAME}
    src/pc.cpp
    src/pg_conn.cpp
    src/conn_util.cpp
    src/query_result.cpp
    src/connection_details.cpp
    src/connections.cpp

    src/tab.cpp

    # Models
    src/model/tab_model.cpp

    # Windows
    src/main_window.cpp
    src/win_new_connection.cpp

    # Resources
    src/resources.c
    )

add_custom_target(resources
  COMMAND glib-compile-resources --target=src/resources.c --generate-source res/resources.gresource.xml
  )

add_dependencies(${EXECUTABLE_NAME} resources)

if ( CMAKE_COMPILER_IS_GNUCC )
  message(STATUS "#warning extension is disabled")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-cpp")
endif ( CMAKE_COMPILER_IS_GNUCC )

link_directories(${GTKMM_LIBRARY_DIRS} ${PQXX_LIBRARY_DIRS} ${GTKSOURCEVIEW_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS} ${PQXX_INCLUDE_DIRS} ${GTKSOURCEVIEW_INCLUDE_DIRS})
target_link_libraries(${EXECUTABLE_NAME} ${GTKMM_LIBRARIES} ${PQXX_LIBRARIES} ${GTKSOURCEVIEW_LIBRARIES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
