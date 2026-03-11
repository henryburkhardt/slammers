import sys
import numpy as np
from collections import defaultdict

# Camera intrinsics(CHANGE FOR DIFFERENT DATASET)
FX, FY, CX, CY = 3408.59, 3408.87, 3117.24, 2064.07

def quat_to_rotmat(qw, qx, qy, qz):
    """Unit quaternion → 3x4 [R | t] rotation block (just R here)."""
    R = np.array([
        [1-2*(qy**2+qz**2),   2*(qx*qy-qz*qw),   2*(qx*qz+qy*qw)],
        [  2*(qx*qy+qz*qw), 1-2*(qx**2+qz**2),   2*(qy*qz-qx*qw)],
        [  2*(qx*qz-qy*qw),   2*(qy*qz+qx*qw), 1-2*(qx**2+qy**2)],
    ])
    return R

def load_images(path):
    """Returns dict: image_id → (R 3x3, t 3, {point2d_idx: point3d_id})"""
    images = {}
    with open(path) as f:
        lines = [l for l in f if not l.startswith('#')]
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if not line:
            i += 1
            continue
        parts = line.split()
        img_id = int(parts[0])
        qw, qx, qy, qz = float(parts[1]), float(parts[2]), float(parts[3]), float(parts[4])
        tx, ty, tz = float(parts[5]), float(parts[6]), float(parts[7])
        R = quat_to_rotmat(qw, qx, qy, qz)
        t = np.array([tx, ty, tz])

        # 2D points
        pts_line = lines[i+1].strip().split()
        obs = {}  # point2d_idx → point3d_id
        for j in range(0, len(pts_line), 3):
            u = float(pts_line[j])
            v = float(pts_line[j+1])
            p3d_id = int(pts_line[j+2])
            if p3d_id != -1:
                obs[j//3] = (u, v, p3d_id)

        images[img_id] = (R, t, obs)
        i += 2
    return images

def load_points3d(path):
    """Returns dict: point3d_id → (x, y, z)"""
    pts = {}
    with open(path) as f:
        for line in f:
            if line.startswith('#') or not line.strip():
                continue
            parts = line.split()
            pid = int(parts[0])
            x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
            pts[pid] = (x, y, z)
    return pts

def convert(images_path, points3d_path, out_path):
    print("Loading images...")
    images = load_images(images_path)
    print(f"  {len(images)} images loaded")

    print("Loading 3D points...")
    pts3d = load_points3d(points3d_path)
    print(f"  {len(pts3d)} 3D points loaded")

    # Build observation list
    observations = []
    for img_id, (R, t, obs_dict) in images.items():
        for p2d_idx, (u, v, p3d_id) in obs_dict.items():
            if p3d_id in pts3d:
                observations.append((img_id, p3d_id, u, v, 0))

    print(f"  {len(observations)} observations")

    with open(out_path, 'w') as f:
        f.write(f"NUM_KEYFRAMES {len(images)}\n")
        for img_id in sorted(images.keys()):
            R, t, _ = images[img_id]
            row = f"{img_id}"
            for r in range(3):
                for c in range(3):
                    row += f" {R[r,c]:.8f}"
                row += f" {t[r]:.8f}"
            row += f" {FX:.4f} {FY:.4f} {CX:.4f} {CY:.4f}"
            f.write(row + "\n")

        f.write(f"NUM_MAPPOINTS {len(pts3d)}\n")
        for pid in sorted(pts3d.keys()):
            x, y, z = pts3d[pid]
            f.write(f"{pid} {x:.8f} {y:.8f} {z:.8f}\n")

        f.write(f"NUM_OBSERVATIONS {len(observations)}\n")
        for (kf_id, mp_id, u, v, oct) in observations:
            f.write(f"{kf_id} {mp_id} {u:.4f} {v:.4f} {oct}\n")

    print(f"Written to {out_path}")
    return images, pts3d

if __name__ == "__main__":
    base = r"SOME_DIR\delivery_area_dslr_undistorted\delivery_area\dslr_calibration_undistorted" # Just an example, point this to the dir where you save the ETH3D dataset files downloaded
    imgs, pts = convert(
        base + r"\images.txt",
        base + r"\points3D.txt",
        base + r"\slam_map.txt"
    )
    print("Done.")