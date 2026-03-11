# ORB-SLAM Project
This part of the repository contains the work put into the ORB-SLAM part of the project. In the ORB folder, you can see all the work that occurred before putting the finished version into ORB-SLAM.
This includes FAST decision tree training, rBRIEF point selection and experiments used to ensure other parts of ORB are working correctly.

The ros2_ws folder contains the ORB-SLAM port, along with other packages that we thought would be useful and programmed, but were less practical than hypothesized.
## How to run.
First, enter the orb_slam/Thirdparty folder in a terminal window. Then enter the DBoW2 folder and run the rebuild.sh script with ```./rebuild.sh```. Do the same for g2o running the same command to rebuild this library. Back out into the orb_slam folder and enter Data. Unzip the newORBvoc.txt.tar.gz file to finalize the setup.

You can then run the orb_slam package. Back out so your working directory is the ros2_ws folder, source your ROS installation, and use the command

```colcon build```.

You should now source the local folder using

```source install/setup.bash```

Finally, run ORB-SLAM with the following command.

```ros2 run orb_slam orb_slam Data/newORBvoc.txt Data/Settings.yaml.```

We also include a settings file for use with the EuRoC SLAM dataset. To use the system with a different dataset, you may need to create a new configuration file for that camera. There is an unfinished calibration package in camera that can be used to get calibration parameters for a new camera. To view the output, you can use rviz as included with ROS2. A configuration file is given for rviz to be configured. To run rviz, use

```ros2 run rviz2 rviz2```

Unfortunately, due to GitHub size limits, we cannot distribute any demo footage. To obtain the demo footage distributed with ORB-SLAM, download it from the original ORB-SLAM repository and use [rosbags](https://ternaris.gitlab.io/rosbags/topics/convert.html) to convert it. 
