## An edge exists IF: 

### (case 1)
- the robot moves from x_i to x_{i+1}
- either from odometry or ICP (scan mathcing)

This gives an odometry esitmate: 
- we need to count the distance and angle based on the rotation of the wheels to get a new pose (3d vector)

### (case 2)
- the robot observes the same part of the enviroment from x_i and x_j
- this is loop closure 
- overlaing to scans with ICP yields a "virual observation" 
- we get a virutal measurment of the position of x_j as seen from x_i

## trsanformations 
- described in homegenous coordiantes 



