# LiDAR-based SLAM
Authors: [Henry Burkahrdt](mailto:henryburkhardt@proton.me) and [Raymond Kim](kimr3@carleton.edu)

Our impliemntation of LiDAR-based posegraph SLAM uses a frontend/backend structure to construct, optimize, and visualize posegraphs built from a Turtlebot 4's sensor data stream. 

When a Turtlebot is conencted, this code allows the Turtlebot to drive around and generate a map of an unknown enviroment. Our SLAM implimention features icp-based loop closure detection and Gauss-Newton graph optimization to build accurate grid maps of unknown enviroments. 

More detail can be found in the /frontend and /backend READMEs. 
