import time
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

N_BEAMS = 360
angles = np.linspace(-np.pi, np.pi, N_BEAMS, endpoint=False)



G2O_PATH = Path("./data/graph.txt")
SCAN_DIR = Path("./data/scans")
UPDATE_MS = 200

# -----------------------------
# G2O parsing
# -----------------------------
def parse_g2o(path: Path):
    vertices = {}
    edges = []

    if not path.exists():
        return vertices, edges

    with path.open() as f:
        for line in f:
            parts = line.strip().split()
            if not parts:
                continue

            if parts[0] == "VERTEX_SE2":
                vid = int(parts[1])
                x, y, theta = map(float, parts[2:5])
                vertices[vid] = (x, y, theta)

            elif parts[0] == "EDGE_SE2":
                v1 = int(parts[1])
                v2 = int(parts[2])
                edges.append((v1, v2))

    return vertices, edges


# -----------------------------
# Plot setup
# -----------------------------
fig, ax = plt.subplots()
ax.set_title("Live g2o Graph + LiDAR")
ax.set_aspect("equal")
ax.grid(True)

pose_scat = ax.scatter([], [], s=20, c="blue", label="poses")
scan_scat = ax.scatter([], [], s=2, c="red", alpha=0.4, label="lidar")
lines = []

ax.legend(loc="upper right")


# -----------------------------
# Update loop
# -----------------------------
def update(frame):
    global lines

    vertices, edges = parse_g2o(G2O_PATH)

    if not vertices:
        return pose_scat, scan_scat

    # --- poses ---
    xs = []
    ys = []
    for vid, (x, y, _) in vertices.items():
        xs.append(x)
        ys.append(y)

    pose_scat.set_offsets(np.column_stack([xs, ys]))

    # --- edges ---
    for ln in lines:
        ln.remove()
    lines.clear()

    for v1, v2 in edges:
        if v1 in vertices and v2 in vertices:
            x1, y1, _ = vertices[v1]
            x2, y2, _ = vertices[v2]
            ln, = ax.plot([x1, x2], [y1, y2], "k-", linewidth=1)
            lines.append(ln)

    # --- lidar scans ---
    scan_x = []
    scan_y = []

    for vid, (px, py, theta) in vertices.items():
        scan_file = SCAN_DIR / f"{vid}.npz"
        if not scan_file.exists():
            continue

        try:
            data = np.load(scan_file)
            ranges = data["ranges"]
            angles = data["angles"]
        except Exception:
            continue  # skip partial writes

        mask = np.isfinite(ranges)
        ranges = ranges[mask]
        angles = angles[mask]

        xs = px + ranges * np.cos(angles + theta)
        ys = py + ranges * np.sin(angles + theta)

        scan_x.extend(xs)
        scan_y.extend(ys)

    if scan_x:
        scan_scat.set_offsets(np.column_stack([scan_x, scan_y]))

    ax.relim()
    ax.autoscale_view()
    ax.margins(x=1.8, y=1.8)

    return pose_scat, scan_scat, *lines


ani = FuncAnimation(fig, update, interval=UPDATE_MS, blit=False)
plt.show()
