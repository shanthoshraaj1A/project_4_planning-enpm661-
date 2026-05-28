# Project 4 - MoveIt Pick and Place Demo

A ROS 2 Humble and MoveIt project for the Franka Panda arm that performs a pick-and-place style sequence in RViz.  
Author: Shanthosh

## Overview

This project implements the ENPM661 Project 4 pick-and-place task using the Franka Panda robot in MoveIt. The robot moves from a home position to goal 1, closes the gripper, moves to goal 2, opens the gripper, and returns to the home position while avoiding collision objects in the scene.

## Dependencies

- Ubuntu 22.04
- ROS 2 Humble
- MoveIt 2
- `colcon`
- `xacro`
- Franka Panda description package
- Panda MoveIt configuration package generated using MoveIt Setup Assistant

Example setup commands:

```bash
source /opt/ros/humble/setup.bash
```

If needed, install common ROS 2 build tools:

```bash
sudo apt install python3-colcon-common-extensions
```

## Run Instructions

1. Create a ROS 2 workspace and place the required packages inside `src/`.

```bash
mkdir -p ~/project4_ws/src
cd ~/project4_ws/src
```

2. Make sure the workspace contains:

- `panda_description`
- your generated Panda MoveIt config package
- `package_uid`

3. Build the package.

```bash
cd ~/project4_ws
conda deactivate
source /opt/ros/humble/setup.bash
colcon build --packages-select package_uid
source install/setup.bash
```

4. Run the project.

If your generated MoveIt config package is named `panda_moveit_config`:

```bash
ros2 launch package_uid pick_place_demo.launch.py
```

If your generated MoveIt config package has a different name:

```bash
ros2 launch package_uid pick_place_demo.launch.py moveit_config_package:=your_config_package_name
```

## Outputs

The program produces:

- RViz visualization of the Franka Panda robot
- Motion planning and execution from `home -> goal1 -> goal2 -> home`
- Gripper closing at goal 1 and opening at goal 2
- Collision objects in the planning scene
- Terminal logs showing planning and execution progress

Example terminal output includes:

- `Added collision objects to the planning scene.`
- `Completed move to home.`
- `Completed move to goal 1.`
- `Completed move to goal 2.`
- `Pick-and-place style sequence completed successfully.`

## Project Structure

```text
package_uid/
├── CMakeLists.txt
├── package.xml
├── README.md
├── config/
│   └── pick_place_demo.yaml
├── launch/
│   └── pick_place_demo.launch.py
└── src/
    └── pick_place_demo.cpp
```

