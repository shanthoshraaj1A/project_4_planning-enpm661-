# package_uid

This package implements the ENPM661 Project 4 requirement using ROS 2 Humble and MoveIt for the Franka Panda arm. The node performs a pick-and-place style sequence in RViz by moving the Panda arm from a home state to goal 1, closing the gripper, moving to goal 2, opening the gripper, and returning to the original home state while planning around obstacles added to the scene.

## What is included

- `src/pick_place_demo.cpp`: C++ MoveIt node for the Panda arm and gripper.
- `launch/pick_place_demo.launch.py`: Launches the generated Panda MoveIt config package and then starts the demo node.
- `config/pick_place_demo.yaml`: Joint-space targets for home, goal 1, goal 2, and the gripper open/close states.

## Required workspace layout

Follow the project setup from the assignment:

1. Create a ROS 2 workspace, for example `project4_ws`.
2. Copy `panda_description` from `moveit_resources` into the workspace `src` folder.
3. Use the MoveIt Setup Assistant to generate a Panda MoveIt config package in the same workspace.
4. Copy this package folder into `project4_ws/src/`.

The workspace should look like this before building:

```text
project4_ws/
  src/
    panda_description/
    panda_moveit_config/        # or your generated config package name
    package_uid/
```

## Build

From the workspace root:

```bash
source /opt/ros/humble/setup.bash
colcon build --packages-select package_uid
source install/setup.bash
```

## Run

If your generated MoveIt config package is named `panda_moveit_config`, run:

```bash
ros2 launch package_uid pick_place_demo.launch.py
```

If you used a different config package name in the Setup Assistant, run:

```bash
ros2 launch package_uid pick_place_demo.launch.py moveit_config_package:=your_config_package_name
```

## Notes for submission

- Submit only this package folder as `package_UID.zip`, not the generated MoveIt config package and not `panda_description`.
- The package name is currently `package_uid` as a safe placeholder. Rename the folder and the package metadata to match your actual UID before final submission.
- Update the maintainer name and email in `package.xml`.
- Record the required videos:
  - Panda setup in the MoveIt Setup Assistant.
  - RViz pick-and-place execution showing home -> goal 1 -> goal 2 -> home.
- Include the required Google Drive or YouTube links and your GitHub repository link in `Firstname_UID.pdf`.

## Tuning

If your generated Panda config uses slightly different limits or the robot needs different joint targets, edit `config/pick_place_demo.yaml`.
