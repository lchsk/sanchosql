cmake_minimum_required(VERSION 2.8)

if (WIN32)
  message(STATUS "Detected Windows")
  # Needed to link this lib on Windows
  set(EXTRA_TARGET_LIB "-lws2_32")
else ()
  message(STATUS "Detected non-Windows")
  set(EXTRA_TARGET_LIB "")
endif ()

project(sanchosql-tests)

message(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})
message(STATUS "CMAKE_SYSTEM_NAME          = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM})
message(STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_CXX_COMPILER_ID      = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_COMPILER_IS_GNUCXX   = ${CMAKE_COMPILER_IS_GNUCXX}")

message(STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH})
message(STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH})


set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig)

pkg_check_modules(GTKSOURCEVIEW gtksourceviewmm-3.0)
message(STATUS "GTKSOURCEVIEWMM_VERSION    = ${GTKSOURCEVIEW_VERSION}")

pkg_check_modules(GTKMM gtkmm-3.0)
message(STATUS "GTKMM_VERSION              = ${GTKMM_VERSION}")

pkg_check_modules(PQXX libpqxx)
message(STATUS "PQXX_VERSION               = ${PQXX_VERSION}")

pkg_check_modules(GTEST gtest)
message(STATUS "GTEST_VERSION              = ${GTEST_VERSION}")

pkg_check_modules(GMOCK gmock)
message(STATUS "GMOCK_VERSION              = ${GMOCK_VERSION}")

message(STATUS "All dependencies look ok")

message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )
message(STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )

################################################################################
#
#                                 Unit tests
#
################################################################################

add_executable(test_string
  ${PROJECT_SOURCE_DIR}/unit_tests/test_string.cpp
  ${PROJECT_SOURCE_DIR}/../src/string.cpp
  )

# add_executable(test_cell_update
#   test_cell_update.cpp
#   ../src/string.cpp
#   ../src/conn_util.cpp
#   ../src/query_result.cpp
#   ../src/connection_details.cpp
#   ../src/connections.cpp
#   ../src/pg_conn.cpp
#   ../src/model/list_tab_model.cpp
#   )

if ( CMAKE_COMPILER_IS_GNUCC )
  message(STATUS "#warning extension is disabled")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-cpp -g")
endif ( CMAKE_COMPILER_IS_GNUCC )

link_directories(${GTKMM_LIBRARY_DIRS} ${PQXX_LIBRARY_DIRS} ${GTKSOURCEVIEW_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS} ${PQXX_INCLUDE_DIRS} ${GTKSOURCEVIEW_INCLUDE_DIRS})

target_link_libraries(test_string
  ${GTKMM_LIBRARIES}
  ${GTEST_LIBRARIES}
  )

# target_link_libraries(test_cell_update
#   ${GTKMM_LIBRARIES}
#   ${GTEST_LIBRARIES}
#   ${GMOCK_LIBRARIES} -lgmock
#   ${PQXX_LIBRARIES} -lpq
#   ${EXTRA_TARGET_LIB}
#   ${GTKSOURCEVIEW_LIBRARIES}
#   -pthread
#   )

enable_testing()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/build/)

add_test("test_string" "test_string")
