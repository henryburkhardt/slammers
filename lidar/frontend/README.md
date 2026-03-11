## LiDAR-based SLAM Frontend 

This code conencts to the robot, and runs the SLAM process (via the `slam_frontend.py` file). It dumps lidar data and maintains the posegraph in `./data`. It also contains the pose-graph data structure (in `graph.py`), point cloud registration algorithms (in `icp.py`), and various utilities to visualize the output of the SLAM run. 

This code does NOT contain optimization functionality (this can be found in .../backend).