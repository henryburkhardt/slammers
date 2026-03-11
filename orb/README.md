# ORB-SLAM Project
This part of the repository contains the work put into the ORB-SLAM part of the project. In the ORB folder, you can see all the work that occurred before putting the finished version into ORB-SLAM.
This includes FAST decision tree training, rBRIEF point selection and experiments used to ensure other parts of ORB are working correctly.

The ros2_ws folder contains the ORB-SLAM port, along with other packages that we thought would be useful and programmed, but were less practical than hypothesized.
## How to run.
First, enter the orb_slam/Thirdparty folder in a terminal window. Then enter the DBoW2 folder and run the rebuild.sh script with ```./rebuild.sh```. Do the same for g2o running the same command to rebuild this library.

You can then run the orb_slam package. Back out so your working directory is the ros2_ws folder, source your ROS installation, and use the command

```colcon build```.

You should now source the local folder using

```source install/setup.bash```

Finally, 
