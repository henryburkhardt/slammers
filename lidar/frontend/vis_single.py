import numpy as np
import matplotlib.pyplot as plt
import sys
from pathlib import Path

def load_and_plot(scan_numer: int):
    data = np.load(f"./data/lidar/{scan_numer}.npz")

    if "ranges" not in data or "angles" not in data:
        raise ValueError("NPZ file must contain 'ranges' and 'angles' arrays")

    ranges = data["ranges"]
    angles = data["angles"]

    if ranges.shape != angles.shape:
        raise ValueError("ranges and angles must have the same shape")

    # Polar → Cartesian
    x = ranges * np.cos(angles)
    y = ranges * np.sin(angles)

    plt.figure(figsize=(6, 6))
    plt.scatter(x, y, s=2)
    plt.gca().set_aspect("equal", adjustable="box")
    plt.xlabel("X (m)")
    plt.ylabel("Y (m)")
    plt.title(f"Lidar Scan:{scan_numer}")
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python view_scan.py scan.npz")
        sys.exit(1)

    load_and_plot(sys.argv[1])
