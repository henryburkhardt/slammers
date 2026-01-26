import numpy as np
import matplotlib.pyplot as plt
import time
from pathlib import Path

INPUT_FILE = Path("slam_output.npz")
POLL_HZ = 2.0  # visualization refresh rate

plt.ion()
fig, ax = plt.subplots(figsize=(6, 6))

def load_keyframes():
    if not INPUT_FILE.exists():
        return None

    data = np.load(INPUT_FILE, allow_pickle=True)
    return data["poses"], data["scans"], data["angles"]

def draw_map(poses, scans, angles):
    ax.clear()

    all_pts = []
    traj_x, traj_y = [], []

    for (x, y, th), scan, ang in zip(poses, scans, angles):
        scan = np.asarray(scan, dtype=np.float32)
        ang  = np.asarray(ang,  dtype=np.float32)

        if scan.ndim != 1 or ang.ndim != 1 or scan.shape != ang.shape:
            continue

        valid = np.isfinite(scan)

        r = scan[valid]
        a = ang[valid]

        xs = r * np.cos(a)
        ys = r * np.sin(a)

        X = x + xs * np.cos(th) - ys * np.sin(th)
        Y = y + xs * np.sin(th) + ys * np.cos(th)

        all_pts.append(np.column_stack((X, Y)))
        traj_x.append(x)
        traj_y.append(y)

    if all_pts:
        pts = np.vstack(all_pts)
        ax.scatter(pts[:, 0], pts[:, 1], s=1, c='black', alpha=0.5)

    ax.plot(traj_x, traj_y, 'r-', linewidth=1.5)
    ax.set_aspect('equal', 'box')
    ax.set_title(f"Live Map ({len(poses)} keyframes)")
    ax.set_xlabel("X [m]")
    ax.set_ylabel("Y [m]")

    plt.draw()
    plt.pause(0.001)

def main():
    last_count = -1

    plt.show(block=False)

    while True:
        loaded = load_keyframes()
        if loaded is not None:
            poses, scans, angles = loaded
            if len(poses) != last_count:
                draw_map(poses, scans, angles)
                last_count = len(poses)

        plt.pause(1.0 / POLL_HZ)


if __name__ == "__main__":
    main()
