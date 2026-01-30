from dataclasses import dataclass
import numpy as np
import math


def se2_relative_transformation(m1:np.ndarray, m2:np.ndarray):
    """relative transformation FROM m1 TO m2"""
    return np.linalg.inv(m1) @ m2
    
def t2v(t_matrix: np.ndarray):
    x = t_matrix[0, 2]
    x = t_matrix[1, 2]
    x = np.arctan2(t_matrix[1, 0], t_matrix[0, 0])
    
    """translation to vector"""
# -----------------------------
# Pose
# -----------------------------
@dataclass
class Pose2D:
    """Represents a 2D robot pose (x, y, theta)."""
    x: float
    y: float
    theta: float = 0.0

class Edge:
    """SE(2) relative pose constraint between two vertices."""
    """the syntax is implied FROM v1 TO v2"""
    def __init__(self, v1_key: int, v2_key: int, t_matrix:np.ndarray, information: np.ndarray):
        
        self.v1_key = v1_key
        self.v2_key = v2_key
        self.t_matrix = t_matrix

        # 3x3 information (inverse covariance) matrix
        self.information = information
    
    def as_vector(self):
        """get edge as (dx, dy, dtheta) vector"""
        dx_rel = self.t_matrix[0, 2]
        dy_rel = self.t_matrix[1, 2]
        dtheta_rel = np.arctan2(self.t_matrix[1, 0], self.t_matrix[0, 0])
    
    def inverse(self):
        return Edge(self.v1_key. self.v2_key, np.linalg.inv(self.t_matrix), self.information)
        
        
class Vertex:
    def __init__(self, key: int, pose: Pose2D, scan, angles):
        self.key = key
        self.pose = pose
        self.scan = scan
        self.angles = angles
        self.neighbors = {}  # {neighbor_key: edge_data}

    def add_neighbor(self, neighbor_key, edge):
        self.neighbors[neighbor_key] = edge

    def get_neighbors(self):
        return self.neighbors.keys()
    
    def to_matrix(self):
        """homogenous matrix rep. of the point"""
        return np.array([
            [np.cos(self.pose.theta), -np.sin(self.pose.theta), self.pose.x],
            [np.sin(self.pose.theta),  np.cos(self.pose.theta), self.pose.y],
            [0,              0,             1]
        ])

    def __str__(self):
        return f"Vertex(key={self.key}, pose={self.pose}, neighbors={list(self.neighbors.keys())})"


class PoseGraph:
    def __init__(self):
        self.vert_list = {}
        self.num_vertices = 0

    def add_vertex(self, key: int, pose: Pose2D, scan=None, angles=None):
        """
        Add a vertex with a given pose.
        """
        if not isinstance(pose, Pose2D):
            raise TypeError("pose must be a Pose2D object")

        if key in self.vert_list:
            raise ValueError(f"Vertex {key} already exists")

        v = Vertex(key, pose, scan, angles)
        self.vert_list[key] = v
        self.num_vertices += 1
        return v    

    def get_vertex(self, key: int) -> Vertex:
        return self.vert_list.get(key)

    def add_edge(self, v1_key: int, v2_key: int, information: np.ndarray, bidirectional=True):
        # QUESTION: Edges should be bi-directional ??
        
        # check that vertex exists
        if v1_key not in self.vert_list or v2_key not in self.vert_list:
            raise KeyError("Both vertices must exist before adding an edge")
        
        # compute the dx, dy and dtheta between the edges
        v1_matrix: np.ndarray = self.get_vertex(v1_key).to_matrix()
        v2_matrix: np.ndarray = self.get_vertex(v2_key).to_matrix()
        
        # QUESTION: should these be absolute value ?? what direction is this implying ?? 
        # dx = v1.pose.x - v2.pose.x
        # dy = v1.pose.y - v2.pose.y
        # dtheta = v1.pose.theta - v2.pose.theta
        
        t_matrix = se2_relative_transformation(v1_matrix, v2_matrix)
        
        new_edge = Edge(v1_key, v2_key, t_matrix)

        self.vert_list[v1_key].add_neighbor(v2_key, new_edge)

        if bidirectional:
            self.vert_list[v2_key].add_neighbor(v1_key, new_edge.inverse())


    def __iter__(self):
        return iter(self.vert_list.values())

    def __str__(self):
        return f"Graph(vertices={self.num_vertices}, keys={list(self.vert_list.keys())})"
    
    def save_lidar_scan(self, filepath: str = "lidar.txt"):
        return

    def save_g2o(self, filepath: str):
        """
        Save the pose graph to a g2o-like file (2D SE2).
        """
        with open("lidar.txt", "w") as f:
            # --- Write vertices ---
            for key, vertex in self.vert_list.items():
                scan = vertex.scan
                angles = vertex.angles

                f.write(
                    f"POINTCLOUD {key} {scan} {angles}\n"
                )

        with open(filepath, "w") as f:
            # --- Write vertices ---
            for key, vertex in self.vert_list.items():
                p = vertex.pose
                f.write(
                    f"VERTEX_SE2 {key} {p.x:.6f} {p.y:.6f} {p.theta:.6f}\n"
                )

            # --- Write edges ---
            for from_key, vertex in self.vert_list.items():
                for to_key, edge in vertex.neighbors.items():
                    dx, dy, dtheta = edge.dx, edge.dy, edge.dtheta
                    info_xx, info_yy, info_tt = edge.information

                    # g2o expects upper-triangular information matrix
                    f.write(
                        "EDGE_SE2 {} {} {:.6f} {:.6f} {:.6f} "
                        "{:.6f} 0.0 0.0 {:.6f} 0.0 {:.6f}\n".format(
                            from_key,
                            to_key,
                            dx, dy, dtheta,
                            info_xx,
                            info_yy,
                            info_tt
                        )
                    )
