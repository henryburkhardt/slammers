from graph import Graph, Vertex, Pose2D, Edge

g = Graph()


first_pose = Pose2D(1, 0, 0)

g.add_vertex(key=1, pose=first_pose, scan=[1,0,0])
g.add_vertex(key=2, pose=first_pose, scan=[2,0,0])

e = Edge(1, 2, 3)
g.add_edge(1, 2, e)


g.save_g2o(filepath="./out.txt")