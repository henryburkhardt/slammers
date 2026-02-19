from dataclasses import dataclass
import numpy as np
import math
from utils import t2v, se2_relative_transformation
from icp import ndt_icp2

    

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
            self.pose.x,
            self.pose.y,
            1
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

    def add_edge(self, v1_key: int, v2_key: int, t_matrix: np.ndarray, information: np.ndarray):
        # QUESTION: Edges should be bi-directional ??
        
        # check that vertex exists
        if v1_key not in self.vert_list or v2_key not in self.vert_list:
            raise KeyError("Both vertices must exist before adding an edge")
    
        
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
                    
    def update_from_g2o(self, filepath: str, vertex_exist: bool = True):
        """
        Update or build a pose graph from a g2o file.

        If vertex_exist=True:
            - Assumes vertices and edges already exist
            - Only updates values

        If vertex_exist=False:
            - Creates vertices and edges as needed
            - Builds the graph from scratch
        """

        with open(filepath, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue

                tokens = line.split()
                tag = tokens[0]

                # -----------------------
                # VERTEX
                # -----------------------
                if tag == "VERTEX_SE2":
                    key = int(tokens[1])
                    x = float(tokens[2])
                    y = float(tokens[3])
                    theta = float(tokens[4])

                    if vertex_exist:
                        # Update existing vertex
                        vertex = self.vert_list[key]
                        vertex.pose.x = x
                        vertex.pose.y = y
                        vertex.pose.theta = theta
                    else:
                        # Create vertex if missing
                        if key not in self.vert_list:
                            vertex = Vertex(key)          # adjust constructor as needed
                            self.vert_list[key] = vertex
                        else:
                            vertex = self.vert_list[key]

                        vertex.pose.x = x
                        vertex.pose.y = y
                        vertex.pose.theta = theta

                # -----------------------
                # EDGE
                # -----------------------
                elif tag == "EDGE_SE2":
                    from_key = int(tokens[1])
                    to_key = int(tokens[2])

                    dx = float(tokens[3])
                    dy = float(tokens[4])
                    dtheta = float(tokens[5])

                    info_xx = float(tokens[6])
                    info_yy = float(tokens[9])
                    info_tt = float(tokens[11])

                    if vertex_exist:
                        # Update existing edge
                        edge = self.vert_list[from_key].neighbors[to_key]
                    else:
                        # Ensure both vertices exist
                        if from_key not in self.vert_list:
                            self.vert_list[from_key] = Vertex(from_key)
                        if to_key not in self.vert_list:
                            self.vert_list[to_key] = Vertex(to_key)

                        # Create edge if missing
                        if to_key not in self.vert_list[from_key].neighbors:
                            edge = Edge(from_key, to_key)  # adjust constructor as needed
                            self.vert_list[from_key].neighbors[to_key] = edge
                        else:
                            edge = self.vert_list[from_key].neighbors[to_key]

                    # Update edge values
                    edge.dx = dx
                    edge.dy = dy
                    edge.dtheta = dtheta
                    edge.information = (info_xx, info_yy, info_tt)
