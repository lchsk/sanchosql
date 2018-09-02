cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(cotire)

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

project(sanchosql)

set(PROJECT_VERSION "0.0.1")

# cpack
set(CPACK_PACKAGE_CONTACT "sanchosql.com")
set(CPACK_GENERATOR "DEB;RPM;ZIP;TGZ")
set(CPACK_PACKAGE_DESCRIPTION, "Client application for PostgreSQL")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# deb stuff
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://sanchosql.com")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libpqxx-dev (>= 4.0), libgtkmm-3.0-dev (>= 3.0), libgtksourceviewmm-3.0-dev (>= 3.0)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "sanchosql.com")

# rpm stuff
# set(CPACK_RPM_PACKAGE_LICENSE "")
set(CPACK_RPM_PACKAGE_URL "http://sanchosql.com")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Client application for PostgreSQL")
set(CPACK_RPM_PACKAGE_REQUIRES "libpqxx >= 4.0, gtkmm30 >= 3.0, gtksourceviewmm3 >= 3.0")

include(CTest)
include(CPack)

MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )
MESSAGE(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )
message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}"         )
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}"     )
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}"  )

MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )


set(EXECUTABLE_NAME "sanchosql")

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  message(STATUS "Setting definition -O0")
  add_definitions(-O0)
  set(CMAKE_CXX_FLAGS "-g -Wall -pedantic")
else()
  message(STATUS "Setting definition -O3")
  add_definitions(-O3)
  set(CMAKE_CXX_FLAGS "-Wall -pedantic")
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
  COMMAND glib-compile-resources --target=${PROJECT_SOURCE_DIR}/src/resources.c --generate-source ${PROJECT_SOURCE_DIR}/res/resources.gresource.xml
  )
add_custom_target(resources
  COMMAND glib-compile-resources --target=${PROJECT_SOURCE_DIR}/src/resources.c --generate-source ${PROJECT_SOURCE_DIR}/res/resources.gresource.xml
  )

add_executable(${EXECUTABLE_NAME}
    src/sanchosql.cpp
    src/pg_conn.cpp
    src/connections.cpp
    src/tab.cpp
    src/string.cpp

    # Models
    src/model/list_tab_model.cpp

    # Windows
    src/main_window.cpp
    src/win_new_connection.cpp

    # Resources
    src/resources.c
    sanchosql.desktop
    res/icons/sanchosql.png
    )

  add_dependencies(${EXECUTABLE_NAME} resources)

  add_library(sancho-system
    src/system/files.cpp
    )
  add_library(sancho-db-core
    src/db/core/conn_util.cpp
    src/db/core/query_result.cpp
    src/db/core/connection_details.cpp
    )

if ( CMAKE_COMPILER_IS_GNUCC )
  message(STATUS "#warning extension is disabled")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-cpp")
endif ( CMAKE_COMPILER_IS_GNUCC )

link_directories(
  ${GTKMM_LIBRARY_DIRS}
  ${PQXX_LIBRARY_DIRS}
  ${GTKSOURCEVIEW_LIBRARY_DIRS}
  ${PROJECT_SOURCE_DIR}/build
  )
include_directories(
  ${GTKMM_INCLUDE_DIRS}
  ${PQXX_INCLUDE_DIRS}
  ${GTKSOURCEVIEW_INCLUDE_DIRS}
  )
target_link_libraries(
  ${EXECUTABLE_NAME}
  ${GTKMM_LIBRARIES}
  ${PQXX_LIBRARIES}
  -lpq
  ${EXTRA_TARGET_LIB}
  ${GTKSOURCEVIEW_LIBRARIES}
  sancho-system
  sancho-db-core
  )
cotire(${EXECUTABLE_NAME})

# Installation

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
install(FILES sanchosql.desktop DESTINATION share/applications)
install(FILES res/icons/sanchosql.png DESTINATION share/pixmaps)

# Uninstall

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  )
