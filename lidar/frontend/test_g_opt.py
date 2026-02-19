from graph import PoseGraph

g = PoseGraph()
g.update_from_g2o(filepath="./data/graph.g2o",vertex_exist=False)