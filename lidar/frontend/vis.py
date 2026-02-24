import time
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

ROT_OFFSET = 0 #TODO: make this alwasy be the first pose
c = np.cos(ROT_OFFSET)
s = np.sin(ROT_OFFSET)
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
    x_min = 0
    x_max = 0
    
    y_min = 0 
    y_max = 0
    
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

pose_scat = ax.scatter([], [], s=10, c="blue", label="poses")
scan_scat = ax.scatter([], [], s=0.5, c="red", alpha=0.4, label="lidar")
first_pose_scat = ax.scatter([], [], s=10, c="green", label="first pose", zorder=6)
first_scan_scat = ax.scatter([], [], s=0.5, c="green", alpha=0.7, label="first scan", zorder=5)

lines = []

ax.legend(loc="upper right")

# Set fixed limits once
ax.set_xlim(-10, 10)
ax.set_ylim(-10, 10)

# (optional) keep equal scaling so SLAM map isn’t distorted
ax.set_aspect('equal', adjustable='box')



# -----------------------------
# Update loop
# -----------------------------
def update(frame):
    global lines

    vertices, edges = parse_g2o(G2O_PATH)

    if not vertices:
        return pose_scat, first_pose_scat, scan_scat

    # get first vertex (in file order)
    first_vid = next(iter(vertices))
    fx, fy, _ = vertices[first_vid]

    # --- poses (excluding first) ---
    xs = []
    ys = []
    
    for vid, (x, y, theta) in vertices.items():
        # rotate pose position into normalized frame
        x_r = c * x - s * y
        y_r = s * x + c * y

        if vid == first_vid:
            fx, fy = x_r, y_r
            continue

        xs.append(x_r)
        ys.append(y_r)

    pose_scat.set_offsets(np.column_stack([xs, ys]) if xs else np.empty((0, 2)))
    first_pose_scat.set_offsets([[fx, fy]])

    # --- edges ---
    for ln in lines:
        ln.remove()
    lines.clear()

    for v1, v2 in edges:
            if v1 in vertices and v2 in vertices:
                x1, y1, _ = vertices[v1]
                x2, y2, _ = vertices[v2]
                
                # rotate both endpoints into normalized frame
                x1_r = c * x1 - s * y1
                y1_r = s * x1 + c * y1
                x2_r = c * x2 - s * y2
                y2_r = s * x2 + c * y2

                ln, = ax.plot([x2_r, x1_r], [y2_r, y1_r], "k-", linewidth=1)
                lines.append(ln)


    # --- lidar scans (optional) ---
    if DRAW_SCANS and SCAN_DIR is not None:
        scan_x = []
        scan_y = []

        first_scan_x = []
        first_scan_y = []
        
        for vid, (px, py, theta) in vertices.items():
            px_r = c * px - s * py
            py_r = s * px + c * py
            theta_r = theta + ROT_OFFSET
            
            scan_file = SCAN_DIR / f"{vid}.npz"
            if not scan_file.exists():
                continue

            try:
                data = np.load(scan_file)
                ranges = data["ranges"]
                angs = data["angles"]
            except Exception:
                continue

            mask = np.isfinite(ranges)
            ranges = ranges[mask]
            angs = angs[mask]

           
            xs = px_r + ranges * np.cos(angs + theta_r)
            ys = py_r + ranges * np.sin(angs + theta_r)

            if vid == first_vid:
                first_scan_x.extend(xs)
                first_scan_y.extend(ys)
            else:
                scan_x.extend(xs)
                scan_y.extend(ys)

        # update regular scans (red)
        if scan_x:
            scan_scat.set_offsets(np.column_stack([scan_x, scan_y]))
        scan_scat.set_visible(True)

        # update first scan (green)
        if first_scan_x:
            first_scan_scat.set_offsets(np.column_stack([first_scan_x, first_scan_y]))
            first_scan_scat.set_visible(True)
        else:
            first_scan_scat.set_visible(False)

    else:
        scan_scat.set_visible(False)
        first_scan_scat.set_visible(False)






    return pose_scat, first_pose_scat, scan_scat, first_scan_scat, *lines



ani = FuncAnimation(fig, update, interval=UPDATE_MS, blit=False)
plt.show()
