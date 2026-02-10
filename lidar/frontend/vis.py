import time
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from env import LIDAR_YAW_OFFSET

N_BEAMS = 360
angles = np.linspace(-np.pi, np.pi, N_BEAMS, endpoint=True)



G2O_PATH = Path("./data/graph.g2o")
# SCAN_DIR = None
SCAN_DIR = Path("./data/lidar")
DRAW_SCANS = True  # set True to draw lidar scans
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

    # --- lidar scans (optional) ---
    if DRAW_SCANS and SCAN_DIR is not None:
        scan_x = []
        scan_y = []

        for vid, (px, py, theta) in vertices.items():
            scan_file = SCAN_DIR / f"{vid}.npz"
            if not scan_file.exists():
                print("cant find file")
                continue

            try:
                data = np.load(scan_file)
                ranges = data["ranges"]
                angles = data["angles"]
            except Exception as e:
                print(e)
                continue  # skip partial writes

            mask = np.isfinite(ranges)
            ranges = ranges[mask]
            angles = angles[mask]

            xs = px + ranges * np.cos(angles + theta + LIDAR_YAW_OFFSET)
            ys = py + ranges * np.sin(angles + theta + LIDAR_YAW_OFFSET)


            scan_x.extend(xs)
            scan_y.extend(ys)

        if scan_x:
            scan_scat.set_offsets(np.column_stack([scan_x, scan_y]))
        scan_scat.set_visible(True)
    else:
        print("no doing scans")
        scan_scat.set_visible(False)

    ax.relim()
    ax.autoscale_view()

    # minimum visible size
    MIN_X_RANGE = 20.0
    MIN_Y_RANGE = 20.0

    # --- X axis ---
    xmin, xmax = ax.get_xlim()
    if xmax - xmin < MIN_X_RANGE:
        cx = 0.5 * (xmin + xmax)
        ax.set_xlim(cx - MIN_X_RANGE / 2,
                    cx + MIN_X_RANGE / 2)

    # --- Y axis ---
    ymin, ymax = ax.get_ylim()
    if ymax - ymin < MIN_Y_RANGE:
        cy = 0.5 * (ymin + ymax)
        ax.set_ylim(cy - MIN_Y_RANGE / 2,
                    cy + MIN_Y_RANGE / 2)

    ax.margins(x=0.1, y=0.1)


    return pose_scat, scan_scat, *lines


ani = FuncAnimation(fig, update, interval=UPDATE_MS, blit=False)
plt.show()
