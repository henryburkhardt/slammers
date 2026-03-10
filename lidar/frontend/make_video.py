"""
make_video.py
Renders the incremental build-up of the pose graph + LiDAR scans into an MP4.

Usage:
    python make_video.py \
        --g2o  ./data/graph.g2o \
        --scans ./data/lidar \
        --out   output.mp4 \
        --fps   10
"""

import argparse
import numpy as np
from pathlib import Path

import matplotlib
matplotlib.use("Agg")          # no display needed
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter

# ── defaults ──────────────────────────────────────────────────────────────────
DEFAULT_G2O   = Path("../saved_runs/05_whole_room_loop/graph.g2o")
DEFAULT_SCANS = Path("../saved_runs/05_whole_room_loop/lidar")
DEFAULT_OUT   = Path("output.mp4")
DEFAULT_FPS   = 10
DEFAULT_DPI   = 150

# ── g2o parser ────────────────────────────────────────────────────────────────
def parse_g2o(path: Path):
    vertices = {}   # vid -> (x, y, theta)
    edges    = []   # (v1, v2)
    if not path.exists():
        raise FileNotFoundError(f"G2O file not found: {path}")
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
                edges.append((int(parts[1]), int(parts[2])))
    return vertices, edges


def load_scan(scan_dir: Path, vid: int):
    """Return (xs, ys) arrays in robot-local frame, or empty arrays."""
    f = scan_dir / f"{vid}.npz"
    if not f.exists():
        return np.array([]), np.array([])
    try:
        data   = np.load(f)
        ranges = data["ranges"]
        angs   = data["angles"]
        mask   = np.isfinite(ranges)
        return ranges[mask], angs[mask]
    except Exception:
        return np.array([]), np.array([])


def scan_world(px, py, theta, ranges, angs):
    xs = px + ranges * np.cos(angs + theta)
    ys = py + ranges * np.sin(angs + theta)
    return xs, ys


# ── main ──────────────────────────────────────────────────────────────────────
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--g2o",  default=DEFAULT_G2O,  type=Path)
    ap.add_argument("--scans",default=DEFAULT_SCANS, type=Path)
    ap.add_argument("--out",  default=DEFAULT_OUT,  type=Path)
    ap.add_argument("--fps",  default=DEFAULT_FPS,  type=int)
    ap.add_argument("--dpi",  default=DEFAULT_DPI,  type=int)
    ap.add_argument("--no-scans", action="store_true",
                    help="skip lidar overlay (faster)")
    args = ap.parse_args()

    print(f"Parsing {args.g2o} …")
    all_vertices, all_edges = parse_g2o(args.g2o)
    vid_order = list(all_vertices.keys())   # insertion order = time order
    n_frames  = len(vid_order)
    print(f"  {n_frames} vertices, {len(all_edges)} edges")

    # build edge lookup: edge appears once BOTH endpoints have been added
    edge_set = set()   # edges revealed so far

    # ── figure setup ──────────────────────────────────────────────────────────
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_title("Pose Graph + LiDAR build-up")
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True)

    # compute axis limits from full vertex set
    xs_all = [v[0] for v in all_vertices.values()]
    ys_all = [v[1] for v in all_vertices.values()]
    pad = 6.0
    
    ax.set_xlim(-14, 20)
    ax.set_ylim(-2.9, 16)

    pose_scat       = ax.scatter([], [], s=12,  c="blue",  zorder=4, label="poses")
    current_scat    = ax.scatter([], [], s=40,  c="orange",marker="*", zorder=6, label="current pose")
    scan_scat       = ax.scatter([], [], s=0.8, c="red",   alpha=0.35, zorder=2, label="lidar")
    first_scan_scat = ax.scatter([], [], s=0.8, c="green", alpha=0.6,  zorder=3, label="first scan")
    edge_lines      = []

    # ax.legend(loc="upper right", markerscale=2)

    first_vid   = vid_order[0]
    fx, fy, fth = all_vertices[first_vid]
    # pre-render first scan (stays green throughout)
    if not args.no_scans and args.scans.exists():
        r0, a0 = load_scan(args.scans, first_vid)
        if len(r0):
            xs0, ys0 = scan_world(fx, fy, fth, r0, a0)
            first_scan_scat.set_offsets(np.column_stack([xs0, ys0]))

    # ── per-frame data accumulation ───────────────────────────────────────────
    revealed_vids: list = []
    edge_set_local: set = set()

    def update(frame_idx):
        nonlocal edge_lines

        vid = vid_order[frame_idx]
        revealed_vids.append(vid)

        # reveal edges whose both endpoints are now known
        known = set(revealed_vids)
        for v1, v2 in all_edges:
            if v1 in known and v2 in known:
                edge_set_local.add((v1, v2))

        # ── poses ─────────────────────────────────────────────────────────────
        pxs, pys = [], []
        for v in revealed_vids:
            x, y, _ = all_vertices[v]
            pxs.append(x); pys.append(y)
        pose_scat.set_offsets(np.column_stack([pxs, pys]))

        cx, cy, _ = all_vertices[vid]
        current_scat.set_offsets([[cx, cy]])

        # ── edges ─────────────────────────────────────────────────────────────
        for ln in edge_lines:
            ln.remove()
        edge_lines.clear()
        for v1, v2 in edge_set_local:
            x1, y1, _ = all_vertices[v1]
            x2, y2, _ = all_vertices[v2]
            ln, = ax.plot([x1, x2], [y1, y2], "k-", linewidth=0.7, zorder=1)
            edge_lines.append(ln)

        # ── scans ─────────────────────────────────────────────────────────────
        if not args.no_scans and args.scans.exists():
            sx, sy = [], []
            for v in revealed_vids[1:]:   # skip first (already green)
                px, py, th = all_vertices[v]
                r, a = load_scan(args.scans, v)
                if len(r):
                    wx, wy = scan_world(px, py, th, r, a)
                    sx.extend(wx); sy.extend(wy)
            if sx:
                scan_scat.set_offsets(np.column_stack([sx, sy]))

        ax.set_title(f"Pose Graph + LiDAR  —  vertex {vid}  ({frame_idx+1}/{n_frames})")
        return pose_scat, current_scat, scan_scat, first_scan_scat, *edge_lines

    # ── render ────────────────────────────────────────────────────────────────
    ani = FuncAnimation(fig, update, frames=n_frames, blit=False, repeat=False)

    writer = FFMpegWriter(fps=args.fps, metadata={"title": "Pose Graph"}, bitrate=2000)
    print(f"Writing {args.out}  (fps={args.fps}, dpi={args.dpi}) …")
    ani.save(str(args.out), writer=writer, dpi=args.dpi)
    print("Done.")


if __name__ == "__main__":
    main()