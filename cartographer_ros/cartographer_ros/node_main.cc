/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "absl/memory/memory.h"
#include "cartographer/mapping/map_builder.h"
#include "cartographer_ros/node.h"
#include "cartographer_ros/node_options.h"
#include "cartographer_ros/ros_log_sink.h"
#include "gflags/gflags.h"
#include "tf2_ros/transform_listener.h"

DEFINE_bool(collect_metrics, false,
            "Activates the collection of runtime metrics. If activated, the "
            "metrics can be accessed via a ROS service.");
// collect_metrics Flag를 기본 false로 설정, true로 하면, ROS Service를 통해 값 확인 가능
DEFINE_string(configuration_directory, "",
              "First directory in which configuration files are searched, "
              "second is always the Cartographer installation to allow "
              "including files from there.");
// configuration_directory를 설정, 설정하지 않으면 Cartographer 설치 경로를 탐색
DEFINE_string(configuration_basename, "",
              "Basename, i.e. not containing any directory prefix, of the "
              "configuration file.");
// configuration_basename을 설정, 경로말고 이름만 설정 (경로는 configuration_directory에서)
DEFINE_string(load_state_filename, "",
              "If non-empty, filename of a .pbstream file to load, containing "
              "a saved SLAM state.");
// load_state_filename을 설정, .pbstream을 통해서 저장된 SLAM 상태를 받아올 수 있음
DEFINE_bool(load_frozen_state, true,
            "Load the saved state as frozen (non-optimized) trajectories.");
// load_frozen_state를 설정, 기본값은 true이며, 최적화되지 않은 이동 궤적을 불러올 지 여부
DEFINE_bool(
    start_trajectory_with_default_topics, true,
    "Enable to immediately start the first trajectory with default topics.");
// start_trajectory_with_default_topics를 설정, 기본값은 true
// 기본 topic을 이용하여 첫번째 trajectory를 바로 시작할지 여부??
// TODO. 이를 비활성화하면 수동으로 시작 위치를 지정할 수 있다는 의미인지 확인 필요
DEFINE_string(
    save_state_filename, "",
    "If non-empty, serialize state and write it to disk before shutting down.");
// save_state_filename을 설정, 지정하면 최종 종료 시에 해당 상태를 저장

namespace cartographer_ros {
namespace {
// 왜 namespace 이름이 없이 중첩시켰을까...?
// namespace를 이름 없이 생성하면, 해당 파일에서만 접근이 가능해지고, 다른 파일에선 사용이 불가능해진다.
// C언어의 static과 유사한 역할을 수행 (unnamed namespace 또는 anonymous namespace라고 함)
void Run() {
  constexpr double kTfBufferCacheTimeInSeconds = 10.;
  tf2_ros::Buffer tf_buffer{::ros::Duration(kTfBufferCacheTimeInSeconds)};
  // TF의 결과값을 최대 kTfBufferCacheTimeInSeconds의 시간만큼 저장하도록 설정
  tf2_ros::TransformListener tf(tf_buffer);
  // http://wiki.ros.org/tf2/Tutorials/Writing%20a%20tf2%20listener%20%28C%2B%2B%29
  // 위 문서의 TF listener 참고
  NodeOptions node_options;
  // Cartographer 실행 시 사용할 Options들을 저장할 Structure 생성
  // 주로 TF나 Topic 관련 옵션들 포함

  TrajectoryOptions trajectory_options;
  // Cartographer 동작 시 사용할 Trajectory Options를 저장할 Structure 생성
  // 주로 Frame의 이름, 사용할 데이터 종류, 데이터 개수 등의 옵션들을 저장
  std::tie(node_options, trajectory_options) =
      LoadOptions(FLAGS_configuration_directory, FLAGS_configuration_basename);
  // tuple을 풀어서 node_options와 trajectory_options에 저장
  // 설정한 configuration폴더와 파일 이름에서 loading
  auto map_builder =
      cartographer::mapping::CreateMapBuilder(node_options.map_builder_options);
  // map_builder option을 이용하여 Map Builder 생성
  // option에 따라 PoseGraph2D or PoseGraph3D 생성
  // option에 따라 TrajectoryCollator or Collator 생성
  
  Node node(node_options, std::move(map_builder), &tf_buffer,
            FLAGS_collect_metrics);
  // 해당 Node 내부에서 모든 Publisher, Service, Timer 생성
  if (!FLAGS_load_state_filename.empty()) {
    node.LoadState(FLAGS_load_state_filename, FLAGS_load_frozen_state);
  }

  if (FLAGS_start_trajectory_with_default_topics) {
    node.StartTrajectoryWithDefaultTopics(trajectory_options);
    // 실제 Topic을 Parsing해서 종류별로 할당하는 부분
  }
  // 내부 동작 부분은 살펴보면, Topic을 쭉 받아서 싱크를 맞춘 후, AddSensor 명령을 통해 데이터를 추가해주는 역할만 수행

  ::ros::spin();

  node.FinishAllTrajectories();
  node.RunFinalOptimization();

  if (!FLAGS_save_state_filename.empty()) {
    node.SerializeState(FLAGS_save_state_filename,
                        true /* include_unfinished_submaps */);
  }
}

}  // namespace
}  // namespace cartographer_ros

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  // GLOG Library 활성화
  google::ParseCommandLineFlags(&argc, &argv, true);
  // GFLAGS Argument Parsing

  CHECK(!FLAGS_configuration_directory.empty())
      << "-configuration_directory is missing.";
  CHECK(!FLAGS_configuration_basename.empty())
      << "-configuration_basename is missing.";
  // CHECK는 특정 조건이 False이면 로그를 남기고 코드 실행을 멈춘다.

  ::ros::init(argc, argv, "cartographer_node");
  // Cartographer Node Initialization
  ::ros::start();
  // ROS의 LifeTime을 수동으로 관리할 때 사용하는 실행 명령어
  // 일반적으로는 NodeHandle을 생성하면 자동으로 수행된다.

  cartographer_ros::ScopedRosLogSink ros_log_sink;
  // 해당 Class가 존재하는 동안, GLOG가 ROS로 LOG 메시지 전달하도록 설정
  cartographer_ros::Run();
  // 실제 동작하는 부분
  ::ros::shutdown();
}
