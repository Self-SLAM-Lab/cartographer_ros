# Copyright 2016 The Cartographer Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 2.8.12)  # Ships with Ubuntu 14.04 (Trusty)
# 최소 CMake 버전이 2.8.12가 필요하다는 의미

project(cartographer_ros)
# Project 이름이 cartographer_ros

set(PACKAGE_DEPENDENCIES
  cartographer_ros_msgs
  geometry_msgs
  message_runtime
  nav_msgs
  pcl_conversions
  rosbag
  roscpp
  roslib
  sensor_msgs
  std_msgs
  tf2
  tf2_eigen
  tf2_ros
  urdf
  visualization_msgs
)
# PACKAGE_DEPENDENCIES 라는 변수에 아래의 값들을 할당한다는 의미

if(WIN32)
  set(Boost_USE_STATIC_LIBS FALSE)
endif()
# WINDOW에서 빌드시 사용하는 옵션, 필요 없다고 함

# For yet unknown reason, if Boost is find_packaged() after find_package(cartographer),
# some Boost libraries including Thread are set to static, despite Boost_USE_STATIC_LIBS,
# which causes linking problems on windows due to shared/static Thread clashing.
# PCL also finds Boost. Work around by moving before find_package(cartographer).
find_package(Boost REQUIRED COMPONENTS system iostreams)
# Boost 패키지 여부 확인, REQUIRED는 해당 패키지가 없으면 오류와 함께 빌드를 종료하겠다는 의미
# COMPONENTS는 패키지 내부의 다음 요소들이 있는지 찾고, 없을 시 오류 발생하겠다는 의미
find_package(PCL REQUIRED COMPONENTS common)
# 마찬가지로 PCL 패키지의 여부를 확인하고, 그 내부의 common이라는 요소가 존재하면 패스 아니면 오류 발생

find_package(cartographer REQUIRED)
# cartographer는 패키지가 존재만 하면 오케이
include("${CARTOGRAPHER_CMAKE_DIR}/functions.cmake")
# 환경 변수에 등록된 CARTOGRAPHER_CMAKE_DIR에 있는 functions.cmake를 추가하겠다는 의미

option(BUILD_GRPC "build features that require Cartographer gRPC support" false)
# BUILD_GRPC를 ON or OFF할 수 있는 옵션 제공 (gRPC를 사용할지 말지에 따라 설정)
google_initialize_cartographer_project()
# functions.cmake에 포함된 함수 호출
google_enable_testing()
# functions.cmake에 포함된 함수 호출
set(CARTOGRAPHER_GMOCK_LIBRARIES ${GMOCK_LIBRARIES})
# CARTOGRAPHER_GMOCK_LIBRARIES 에 GMOCK_LIBRARIES 값 할당

find_package(catkin REQUIRED COMPONENTS ${PACKAGE_DEPENDENCIES})
# catkin 패키지 검색 후, 패키지 디펜던시에 있는 요소들이 모두 있는지 확인

include(FindPkgConfig)
# PkgConfig CMake 모듈 추가

find_package(absl REQUIRED)
# absl Package 확인
find_package(LuaGoogle REQUIRED)
# LuaGoogle Package 확인
find_package(Eigen3 REQUIRED)
# Eigen3 Package 확인

find_package(urdfdom_headers REQUIRED)
# urdfdom_header Package 확인
if(DEFINED urdfdom_headers_VERSION)
  if(${urdfdom_headers_VERSION} GREATER 0.4.1)
    add_definitions(-DURDFDOM_HEADERS_HAS_SHARED_PTR_DEFS)
  endif()
endif()
# urdfcom_headers Version에 따라, 컴파일 옵션 추가

include_directories(
  ${urdfdom_headers_INCLUDE_DIRS}
)
# urdf_headers가 포함된 폴더를 검색 폴더에 추가

# Override Catkin's GTest configuration to use GMock.
set(GTEST_FOUND TRUE)
set(GTEST_INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS})
set(GTEST_LIBRARIES ${CARTOGRAPHER_GMOCK_LIBRARIES})
# 각 변수에 값을 할당

catkin_package(
  CATKIN_DEPENDS
    ${PACKAGE_DEPENDENCIES}
  DEPENDS
    # TODO(damonkohler): This should be here but causes Catkin to abort because
    # protobuf specifies a library '-lpthread' instead of just 'pthread'.
    # CARTOGRAPHER
    PCL
    EIGEN3
    Boost
    urdfdom_headers
  INCLUDE_DIRS "."
  LIBRARIES ${PROJECT_NAME}
)
# 현재 폴더를 Include Path로 설정
# 디펜던시로 다른 Catkin 패키지 확인
# 디펜던시로 Catkin이 아닌 다른 패키지들 확인
# Library 결과물을 Project 이름으로 설정

file(GLOB_RECURSE ALL_SRCS "cartographer_ros/*.cc" "cartographer_ros/*.h")
file(GLOB_RECURSE ALL_TESTS "cartographer_ros/*_test.cc")
file(GLOB_RECURSE ALL_EXECUTABLES "cartographer_ros/*_main.cc")
file(GLOB_RECURSE ALL_GRPC_FILES "cartographer_ros/cartographer_grpc/*")
list(REMOVE_ITEM ALL_SRCS ${ALL_TESTS})
list(REMOVE_ITEM ALL_SRCS ${ALL_EXECUTABLES})
# 해당 CMake가 수행되는 디렉토리 및 하위 디렉토리에서 해당 파일 이름에 맞는 파일들을 찾고
# 그 파일들을 각 변수가 할당
# ALL_SRCS에는 cartographer_ros/*.cc, cartographer_ros/*.h 파일이 포함됨
# ALL_SRCS에서 ALL_TESTS와 ALL_EXECUTABLES에 중복되는 값을 지움
if (NOT ${BUILD_GRPC})
  list(REMOVE_ITEM ALL_SRCS ${ALL_GRPC_FILES})
  list(REMOVE_ITEM ALL_TESTS ${ALL_GRPC_FILES})
  list(REMOVE_ITEM ALL_EXECUTABLES ${ALL_GRPC_FILES})
endif()
# BUILD_GRPC 설정에 따라 GRPC 관련 파일들도 리스트에서 지움

add_library(${PROJECT_NAME} STATIC ${ALL_SRCS})
# ALL_SRCS에 포함된 파일들을 정적 링크 라이브러리를 생성

add_subdirectory("cartographer_ros")
# cartographer_ros 폴더를 서브 디렉토리에 추가

target_link_libraries(${PROJECT_NAME} PUBLIC cartographer)

# Lua
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${LUA_INCLUDE_DIR})

# PCL
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${PCL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PCL_LIBRARIES})
set(BLACKLISTED_PCL_DEFINITIONS " -march=native -msse4.2 -mfpmath=sse ")
foreach(DEFINITION ${PCL_DEFINITIONS})
  list (FIND BLACKLISTED_PCL_DEFINITIONS "${DEFINITION}" DEFINITIONS_INDEX)
  if (${DEFINITIONS_INDEX} GREATER -1)
    continue()
  endif()
  set(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS} ${DEFINITION}")
endforeach()

# Eigen
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
  "${EIGEN3_INCLUDE_DIR}")

# Boost
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
  "${Boost_INCLUDE_DIRS}")
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

# Catkin
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${catkin_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${catkin_LIBRARIES})
add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})

# Add the binary directory first, so that port.h is included after it has
# been generated.
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set(TARGET_COMPILE_FLAGS "${TARGET_COMPILE_FLAGS} ${GOOG_CXX_FLAGS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
  COMPILE_FLAGS ${TARGET_COMPILE_FLAGS})
# 컴파일 flags 설정

if (CATKIN_ENABLE_TESTING)
  foreach(TEST_SOURCE_FILENAME ${ALL_TESTS})
    get_filename_component(TEST_NAME ${TEST_SOURCE_FILENAME} NAME_WE)
    catkin_add_gtest(${TEST_NAME} ${TEST_SOURCE_FILENAME})
    # catkin_add_gtest uses a plain (i.e. no PUBLIC/PRIVATE/INTERFACE) call to
    # target_link_libraries. That forces us to do the same.
    target_link_libraries(${TEST_NAME} ${GMOCK_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
    target_include_directories(${TEST_NAME} SYSTEM PUBLIC ${LUA_INCLUDE_DIR})
    target_link_libraries(${TEST_NAME} ${LUA_LIBRARIES})
    target_include_directories(${TEST_NAME} SYSTEM PUBLIC ${catkin_INCLUDE_DIRS})
    target_link_libraries(${TEST_NAME} ${catkin_LIBRARIES})
    add_dependencies(${TEST_NAME} ${catkin_EXPORTED_TARGETS})
    target_link_libraries(${TEST_NAME} cartographer)
    target_link_libraries(${TEST_NAME} ${PROJECT_NAME})
    set_target_properties(${TEST_NAME} PROPERTIES COMPILE_FLAGS ${TARGET_COMPILE_FLAGS})
    # Needed for dynamically linked GTest on Windows.
    if (WIN32)
      target_compile_definitions(${TEST_NAME} PUBLIC -DGTEST_LINKED_AS_SHARED_LIBRARY)
    endif()
  endforeach()
endif()

install(DIRECTORY launch urdf configuration_files
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(PROGRAMS scripts/tf_remove_frames.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

# Install source headers.
file(GLOB_RECURSE HDRS "cartographer_ros/*.h")
foreach(HDR ${HDRS})
  file(RELATIVE_PATH REL_FIL ${PROJECT_SOURCE_DIR} ${HDR})
  get_filename_component(INSTALL_DIR ${REL_FIL} DIRECTORY)
  install(
    FILES
      ${HDR}
    DESTINATION
      include/${INSTALL_DIR}
  )
endforeach()
