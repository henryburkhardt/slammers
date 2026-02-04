from dataclasses import dataclass
import numpy as np
import math
from utils import t2v, se2_relative_transformation
    

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
        return (dx_rel, dy_rel, dtheta_rel)
    
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
        """homogenous matrix representation of the point"""
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

    def add_edge(self, v1_key: int, v2_key: int, information: np.ndarray):
        # QUESTION: Edges should be bi-directional ??
        
        # check that vertex exists
        if v1_key not in self.vert_list or v2_key not in self.vert_list:
            raise KeyError("Both vertices must exist before adding an edge")
        
        # compute the dx, dy and dtheta between the edges
        v1_matrix: np.ndarray = self.get_vertex(v1_key).to_matrix()
        v2_matrix: np.ndarray = self.get_vertex(v2_key).to_matrix()
        
        t_matrix = se2_relative_transformation(v1_matrix, v2_matrix)
        
        new_edge = Edge(v1_key, v2_key, t_matrix, information)

        self.vert_list[v1_key].add_neighbor(v2_key, new_edge)

    def save_graph_to_file(self, filepath: str):
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
            first_node = True

            for key, vertex in self.vert_list.items():
                p = vertex.pose
                f.write(
                    f"VERTEX_SE2 {key} {p.x:.6f} {p.y:.6f} {p.theta:.6f}\n"
                )

                if(first_node):
                    f.write(f"FIX 0\n")
                    first_node = False

            # --- Write edges ---
            for from_key, vertex in self.vert_list.items():
                for to_key, edge in vertex.neighbors.items():
                    dx, dy, dtheta = edge.as_vector()

                    # Extract diagonal entries from 3x3 information matrix
                    info_xx = float(edge.information[0, 0])
                    info_yy = float(edge.information[1, 1])
                    info_tt = float(edge.information[2, 2])

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
    def update_from_g2o(self, filepath: str):
        """
        update an existing pose graph from a g2o file.

        Assumptions:
        - All vertices already exist in self.vert_list
        - All edges already exist in vertex.neighbors
        - topology is identical; only values change
        """

        with open(filepath, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue

                tokens = line.split()
                tag = tokens[0]

                
                # Update vertex poses
                if tag == "VERTEX_SE2":
                    key = int(tokens[1])

                    vertex = self.vert_list[key]  # must exist
                    vertex.pose.x = float(tokens[2])
                    vertex.pose.y = float(tokens[3])
                    vertex.pose.theta = float(tokens[4])

                # Update edge measurements
                elif tag == "EDGE_SE2":
                    from_key = int(tokens[1])
                    to_key = int(tokens[2])

                    edge = self.vert_list[from_key].neighbors[to_key]  # must exist

                    edge.dx = float(tokens[3])
                    edge.dy = float(tokens[4])
                    edge.dtheta = float(tokens[5])

                    # Upper-triangular information matrix
                    edge.information = (
                        float(tokens[6]),   # I_xx
                        float(tokens[9]),   # I_yy
                        float(tokens[11]),  # I_tt
                    )
