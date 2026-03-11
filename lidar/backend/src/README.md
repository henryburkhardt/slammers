## LiDAR-based SLAM Backend 

This code optimizes posegraphs (i.e. it takes in a pose-graph from the frontend, containing many observations about the robots envrioment, and moves around the poses and edges to reduce the error in the graph-- to represent the evniroment more accuratley).

It operates as a local web API: a graph can be passed in (in g2o format), and the service will return an optimized version of the graph. 

For optimzaion, we use the [g2o library](https://github.com/RainerKuemmerle/g2o) to spin up a [Gauss-Newton](https://en.wikipedia.org/wiki/Gauss%E2%80%93Newton_algorithm) optimzier. 