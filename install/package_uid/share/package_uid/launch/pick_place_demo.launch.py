from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    moveit_config_package = LaunchConfiguration("moveit_config_package")
    moveit_config_launch_file = LaunchConfiguration("moveit_config_launch_file")
    parameters_file = LaunchConfiguration("parameters_file")
    demo_start_delay = LaunchConfiguration("demo_start_delay")

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "moveit_config_package",
                default_value="panda_moveit_config",
                description="MoveIt config package generated with the Setup Assistant.",
            ),
            DeclareLaunchArgument(
                "moveit_config_launch_file",
                default_value="demo.launch.py",
                description="Launch file from the generated Panda MoveIt config package.",
            ),
            DeclareLaunchArgument(
                "parameters_file",
                default_value=PathJoinSubstitution(
                    [FindPackageShare("package_uid"), "config", "pick_place_demo.yaml"]
                ),
                description="Parameter file for the arm and gripper goals.",
            ),
            DeclareLaunchArgument(
                "demo_start_delay",
                default_value="5.0",
                description="Seconds to wait before starting the project node.",
            ),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    PathJoinSubstitution(
                        [FindPackageShare(moveit_config_package), "launch", moveit_config_launch_file]
                    )
                )
            ),
            TimerAction(
                period=demo_start_delay,
                actions=[
                    Node(
                        package="package_uid",
                        executable="pick_place_demo",
                        name="pick_place_demo",
                        output="screen",
                        parameters=[parameters_file],
                    )
                ],
            ),
        ]
    )
