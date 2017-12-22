cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE           = " ${CMAKE_BUILD_TYPE})

set(ENV{G_MESSAGES_DEBUG} "all")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  message(STATUS "This is a DEBUG build!")
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
  message(STATUS "This is a RELEASE build!")
endif()

if (WIN32)
  message(STATUS "Detected Windows")
  # Needed to link this lib on Windows
  set(EXTRA_TARGET_LIB "-lws2_32")
else ()
  message(STATUS "Detected non-Windows")
  set(EXTRA_TARGET_LIB "")
endif ()

project(postgres-client)

MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )
message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )

MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )


set(EXECUTABLE_NAME "postgres-client")

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  message(STATUS "Setting definition -O0")
  add_definitions(-O0)
else()
  message(STATUS "Setting definition -O3")
  add_definitions(-O3)
endif()

find_package(PkgConfig)

pkg_check_modules(GTKSOURCEVIEW gtksourceviewmm-3.0)
message(STATUS "GTKSOURCEVIEWMM_VERSION    = ${GTKSOURCEVIEW_VERSION}")

pkg_check_modules(GTKMM gtkmm-3.0)
message(STATUS "GTKMM_VERSION              = ${GTKMM_VERSION}")

pkg_check_modules(PQXX libpqxx)
message(STATUS "PQXX_VERSION               = ${PQXX_VERSION}")

message(STATUS "All dependencies look ok")
message(STATUS "Generating resources...")

MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )
MESSAGE( STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )

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
    src/util.cpp

    # Models
    src/model/list_tab_model.cpp

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
target_link_libraries(${EXECUTABLE_NAME} ${GTKMM_LIBRARIES} ${PQXX_LIBRARIES} -lpq ${EXTRA_TARGET_LIB} ${GTKSOURCEVIEW_LIBRARIES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
