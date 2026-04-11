#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <geometry_msgs/msg/pose.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/msg/collision_object.hpp>
#include <moveit_msgs/msg/move_it_error_codes.hpp>
#include <rclcpp/rclcpp.hpp>
#include <shape_msgs/msg/solid_primitive.hpp>

using namespace std::chrono_literals;

namespace
{

using MoveGroupInterface = moveit::planning_interface::MoveGroupInterface;
using PlanningSceneInterface = moveit::planning_interface::PlanningSceneInterface;
using MoveItErrorCode = moveit::core::MoveItErrorCode;

std::vector<double> get_double_array_parameter(
  const rclcpp::Node::SharedPtr & node,
  const std::string & name,
  const std::vector<double> & default_value)
{
  node->declare_parameter(name, default_value);
  return node->get_parameter(name).as_double_array();
}

double get_double_parameter(
  const rclcpp::Node::SharedPtr & node,
  const std::string & name,
  double default_value)
{
  node->declare_parameter(name, default_value);
  return node->get_parameter(name).as_double();
}

moveit_msgs::msg::CollisionObject make_box(
  const std::string & frame_id,
  const std::string & object_id,
  double x,
  double y,
  double z,
  double size_x,
  double size_y,
  double size_z)
{
  moveit_msgs::msg::CollisionObject object;
  object.id = object_id;
  object.header.frame_id = frame_id;

  shape_msgs::msg::SolidPrimitive primitive;
  primitive.type = primitive.BOX;
  primitive.dimensions = {size_x, size_y, size_z};

  geometry_msgs::msg::Pose pose;
  pose.orientation.w = 1.0;
  pose.position.x = x;
  pose.position.y = y;
  pose.position.z = z;

  object.primitives.push_back(primitive);
  object.primitive_poses.push_back(pose);
  object.operation = object.ADD;
  return object;
}

bool plan_and_execute(
  MoveGroupInterface & move_group,
  const std::vector<double> & target,
  const std::string & label,
  const rclcpp::Logger & logger)
{
  move_group.setStartStateToCurrentState();
  move_group.setJointValueTarget(target);

  MoveGroupInterface::Plan plan;
  const auto plan_result = move_group.plan(plan);
  if (plan_result != MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(logger, "Planning failed for %s.", label.c_str());
    return false;
  }

  const auto execute_result = move_group.execute(plan);
  if (execute_result != MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(logger, "Execution failed for %s.", label.c_str());
    return false;
  }

  RCLCPP_INFO(logger, "Completed %s.", label.c_str());
  return true;
}

}  // namespace

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>(
    "pick_place_demo");

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  std::thread spinner([&executor]() {executor.spin();});

  const auto logger = node->get_logger();
  const std::vector<double> home_joints = get_double_array_parameter(
    node, "home_joints", {0.0, -0.785, 0.0, -2.356, 0.0, 1.571, 0.785});
  const std::vector<double> goal1_joints = get_double_array_parameter(
    node, "goal1_joints", {0.55, -0.95, 0.35, -2.10, 0.20, 1.75, 1.10});
  const std::vector<double> goal2_joints = get_double_array_parameter(
    node, "goal2_joints", {-0.60, -0.90, -0.30, -2.05, -0.10, 1.80, -0.80});
  const std::vector<double> hand_open = get_double_array_parameter(
    node, "hand_open", {0.04, 0.04});
  const std::vector<double> hand_closed = get_double_array_parameter(
    node, "hand_closed", {0.00, 0.00});
  const double pause_seconds = get_double_parameter(node, "pause_seconds", 1.0);

  MoveGroupInterface arm_group(node, "panda_arm");
  MoveGroupInterface hand_group(node, "hand");
  PlanningSceneInterface planning_scene_interface;

  arm_group.startStateMonitor();
  hand_group.startStateMonitor();

  arm_group.setPlanningTime(10.0);
  arm_group.setNumPlanningAttempts(10);
  arm_group.setMaxVelocityScalingFactor(0.4);
  arm_group.setMaxAccelerationScalingFactor(0.4);

  hand_group.setPlanningTime(5.0);
  hand_group.setNumPlanningAttempts(5);
  hand_group.setMaxVelocityScalingFactor(1.0);
  hand_group.setMaxAccelerationScalingFactor(1.0);

  RCLCPP_INFO(logger, "Waiting for MoveIt state and controllers.");
  rclcpp::sleep_for(2s);

  const std::string planning_frame = arm_group.getPlanningFrame();
  std::vector<moveit_msgs::msg::CollisionObject> collision_objects;
  collision_objects.push_back(
    make_box(planning_frame, "table", 0.55, 0.00, -0.05, 0.90, 1.40, 0.08));
  collision_objects.push_back(
    make_box(planning_frame, "left_obstacle", 0.45, 0.28, 0.15, 0.10, 0.10, 0.30));
  collision_objects.push_back(
    make_box(planning_frame, "right_obstacle", 0.45, -0.28, 0.15, 0.10, 0.10, 0.30));

  planning_scene_interface.applyCollisionObjects(collision_objects);
  RCLCPP_INFO(logger, "Added collision objects to the planning scene.");
  rclcpp::sleep_for(1s);

  const auto pause = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(pause_seconds));

  const auto shutdown_and_return = [&](int code) {
    rclcpp::shutdown();
    if (spinner.joinable()) {
      spinner.join();
    }
    return code;
  };

  if (!plan_and_execute(arm_group, home_joints, "move to home", logger)) {
    return shutdown_and_return(1);
  }
  rclcpp::sleep_for(pause);

  if (!plan_and_execute(hand_group, hand_open, "open gripper at home", logger)) {
    return shutdown_and_return(1);
  }
  rclcpp::sleep_for(pause);

  if (!plan_and_execute(arm_group, goal1_joints, "move to goal 1", logger)) {
    return shutdown_and_return(1);
  }
  rclcpp::sleep_for(pause);

  if (!plan_and_execute(hand_group, hand_closed, "close gripper at goal 1", logger)) {
    return shutdown_and_return(1);
  }
  rclcpp::sleep_for(pause);

  if (!plan_and_execute(arm_group, goal2_joints, "move to goal 2", logger)) {
    return shutdown_and_return(1);
  }
  rclcpp::sleep_for(pause);

  if (!plan_and_execute(hand_group, hand_open, "open gripper at goal 2", logger)) {
    return shutdown_and_return(1);
  }
  rclcpp::sleep_for(pause);

  if (!plan_and_execute(arm_group, home_joints, "return to home", logger)) {
    return shutdown_and_return(1);
  }

  RCLCPP_INFO(logger, "Pick-and-place style sequence completed successfully.");
  return shutdown_and_return(0);
}
