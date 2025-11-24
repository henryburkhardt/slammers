from dataclasses import dataclass

@dataclass
class Pose2D:
    """rerpresetnts a 2d robot pose (x, y, r)"""
    x: float
    y: float
    theta: float = 0.0 # rotation
    
    def to_matrix():
        # TOOD: fill in
        pass
    
    def inverse():
        # TODO: fill in
        pass
