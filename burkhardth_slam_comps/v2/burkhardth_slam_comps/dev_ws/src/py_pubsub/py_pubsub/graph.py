import numpy as np 
from dataclasses import dataclass

@dataclass 
class PoseVertex:
    x: float
    y: float 
    theta: float # rotation angle 
    id: int 

@dataclass
class Edge: 
    to: PoseVertex
    frm: PoseVertex
    scan: any 
    information: any # np.identity matrix

class Graph: 
    def __init__(self):
        self.edges = []
        self.nodes = []

    def create_vertex(self, x, y, theta, scan) -> PoseVertex:
        return 

    def create_edge(self, to: PoseVertex, frm: PoseVertex):
        return