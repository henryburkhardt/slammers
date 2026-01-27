from dataclasses import dataclass

# -----------------------------
# Pose
# -----------------------------
@dataclass
class Pose2D:
    """Represents a 2D robot pose (x, y, theta)."""
    x: float
    y: float
    theta: float = 0.0


@dataclass
class Edge:
    dx: float
    dy: float
    dtheta: float
    information: tuple = (1.0, 1.0, 1.0)  # (xx, yy, tt)

# -----------------------------
# Vertex
# -----------------------------
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

    def __str__(self):
        return f"Vertex(key={self.key}, pose={self.pose}, neighbors={list(self.neighbors.keys())})"


class Graph:
    def __init__(self):
        self.vert_list = {}
        self.num_vertices = 0

    def add_vertex(self, key: int, pose: Pose2D, scan=None, angles=None):
        """
        Add a vertex with a given pose and optional scan.
        """
        if not isinstance(pose, Pose2D):
            raise TypeError("pose must be a Pose2D object")

        if key in self.vert_list:
            raise ValueError(f"Vertex {key} already exists")

        v = Vertex(key, pose, scan, angles)
        self.vert_list[key] = v
        self.num_vertices += 1
        return v

    def get_vertex(self, key: int):
        return self.vert_list.get(key)

    def add_edge(self, from_key: int, to_key: int, edge: Edge):
        """
        Add an edge from from_key -> to_key.
        Both vertices must exist and edge must be an EdgeSE2.
        """
        if from_key not in self.vert_list or to_key not in self.vert_list:
            raise KeyError("Both vertices must exist before adding an edge")

        if not isinstance(edge, Edge):
            raise TypeError("edge must be an EdgeSE2 object")

        self.vert_list[from_key].add_neighbor(to_key, edge)

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
