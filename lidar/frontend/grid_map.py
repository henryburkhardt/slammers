"""Gridmap data class, generates grid maps from posegraph + lidar scans"""

from pathlib import Path
import numpy as np 
from math import ceil, floor
from utils import load_and_filter_scan, load_scan
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap




G2O_PATH = Path("./data/graph.g2o")
NUM_SCANS = 10


def parse_g2o(path: Path):
    vertices = {}
    
    x_max = -10e6
    x_min = 10e6
    y_max = -10e6
    y_min = 10e6

    if not path.exists():
        return vertices

    with path.open() as f:
        for line in f:
            parts = line.strip().split()
            if not parts:
                continue

            if parts[0] == "VERTEX_SE2":
                vid = int(parts[1])
                x, y, theta = map(float, parts[2:5])
                vertices[vid] = (x, y, theta)
                
                if x > x_max:
                    x_max = x
                if x < x_min: 
                    x_min = x
                if y > y_max: 
                    y_max = y 
                if y < y_min: 
                    y_min = y

    return vertices, x_max, x_min, y_max, y_min

def bresenham_line(x0, y0, x1, y1):
    """Return list of grid cells from (x0, y0) to (x1, y1) using Bresenham."""
    cells = []

    dx = abs(x1 - x0)
    dy = abs(y1 - y0)

    x, y = x0, y0

    sx = 1 if x1 >= x0 else -1
    sy = 1 if y1 >= y0 else -1

    if dx > dy:
        err = dx / 2.0
        while x != x1:
            cells.append((x, y))
            err -= dy
            if err < 0:
                y += sy
                err += dx
            x += sx
    else:
        err = dy / 2.0
        while y != y1:
            cells.append((x, y))
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy

    cells.append((x1, y1))  # include endpoint
    return cells

class GridMap:
    def __init__(self, x_min, x_max, y_min, y_max, resolution):
        margin = 6

        self.x_min = x_min
        self.x_max = x_max
        self.y_min = y_min
        self.y_max = y_max
        self.resolution = resolution
        
        self.x_min -= margin
        self.x_max += margin
        self.y_min -= margin
        self.y_max += margin

        map_width_m  = self.x_max - self.x_min
        map_height_m = self.y_max - self.y_min
        self.width  = ceil(map_width_m  / resolution)
        self.height = ceil(map_height_m / resolution)
        
        self.width  = ceil(map_width_m  / resolution)   # number of columns
        self.height = ceil(map_height_m / resolution) 
        
        self.grid = np.zeros((self.height, self.width))
        print(f"Grid init at h:{self.height}, w:{self.width} ")
        print(f" Ranges x:({self.x_min},{self.x_max}) y:({self.y_min},{self.y_max})")

        self.L_OCC  = np.log(0.9 / 0.1)   # strong hit confidence
        self.L_FREE = np.log(0.4 / 0.6)     # weaker free evidence
        self.L_MIN  = -5.0
        self.L_MAX  =  5.0
        self.MIN_RANGE = 0.2
        self.MAX_RANGE = 8
        
        return 
    
    def world_to_cell(self, x, y):
        i = floor((x - self.x_min) / self.resolution)   # column index [0 .. width-1]
        j = floor((y - self.y_min) / self.resolution)   # row index    [0 .. height-1]
        return (i, j)
    
    def cell_in_bounds(self, i, j):
        if i < 0 or j < 0:
            return False 
        if i >= self.width or j >= self.height:
            return False 
        return True
    
    def update_from_scan(self, pointcloud, pose):
        (m_x, m_y) = self.world_to_cell(pose[0], pose[1])
        
        for beam in pointcloud:
            angle_world = pose[2] + beam[1]
            r = beam[0] # range
            
            if r < self.MIN_RANGE:
                continue
            
            if r > self.MAX_RANGE or np.isinf(r):
                continue
            
            # endpoint in world coordinates
            x_end = pose[0] + r * np.cos(angle_world)
            y_end = pose[1] + r * np.sin(angle_world)
            
            (i_end, j_end) = self.world_to_cell(x_end, y_end)
            
            cells_on_ray = bresenham_line(m_x, m_y, i_end, j_end)

            for i, j in cells_on_ray[:-1]:  # all but last = free
                if self.cell_in_bounds(i, j):
                    self.grid[j, i] += self.L_FREE

            # last cell = occupied
            i, j = cells_on_ray[-1]
            if self.cell_in_bounds(i, j):
                self.grid[j, i] += self.L_OCC
                
    def get_binary_grid(self):
        p = 1 - 1 / (1 + np.exp(self.grid))  # probability from log-odds

        occupancy = np.full(self.grid.shape, 1)  # default unknown
        occupancy[p > 0.65] = 2  # occupied
        occupancy[p < 0.35] = 0    # free

        # map occupancy values to colormap indices
        # disp = np.full_like(occupancy, 1)  # unknown = 1
        # disp[occupancy == 0] = 0           # free = 0
        # disp[occupancy == 100] = 2         # occupied = 2
        return occupancy
    
    def show(self):
        # define custom colormap: free / unknown / occupied
        cmap = ListedColormap(['lightblue', 'gray', 'darkblue'])
        plt.figure(figsize=(8,8))
        plt.imshow(self.get_binary_grid(), cmap=cmap)
        plt.title("Discrete Occupancy Grid")
        plt.axis('off')
        plt.show()

if __name__ == "__main__":
    graph, x_max, x_min, y_max, y_min = parse_g2o(G2O_PATH)
    num_vertices = len(graph)

    g = GridMap(-2, 2, -7, 2, resolution=0.1)

    for i in range(1, NUM_SCANS):
        pointcloud = load_scan(i)
        pose = graph[i]
        g.update_from_scan(pointcloud, pose)

    g.show()


        
    
