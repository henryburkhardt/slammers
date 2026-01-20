from dataclasses import dataclass
import Pose2D

@dataclass
class DirectedEdge:
    """rerpresetnts a directed edge between two nodes in a pose graph"""
    source: Pose2D
    target: Pose2D
    id: int = 0

    
