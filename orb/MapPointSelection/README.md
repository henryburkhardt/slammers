# MapPoint Selection for Visual SLAM

**Author:** [Yuming Zhang](mailto:zhanga5@carleton.edu)

This implementation of MapPoint Selection takes in a SLAM map and selects the most localization-valuable subset of map points under a fixed budget. It uses a lazy greedy algorithm grounded in Fisher Information theory, with a formal (1 − 1/e) approximation guarantee.

## Files

- `MapPointSelection.cc` - the selection algorithm
- `convert_colmap.py` - converts an ETH3D/COLMAP reconstruction into the format the algorithm expects

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

First, convert your ETH3D dataset. Open `convert_colmap.py` and update the camera intrinsics (found in `cameras.txt` from the ETH3D dataset you downloaded) and the path to your folder. Then run:

```bash
python convert_colmap.py
```

It will generate `slam_map.txt` that `MapPointSelection.cc` could read. Then run the selection:

```bash
./build/MapPointSelection --input slam_map.txt --budget BUDGET_OF_YOUR_CHOICE --output reduced_map.txt
```

You may also run it with `--synthetic` to test without real data:

```bash
./build/MapPointSelection --synthetic --syn_kf KEYFRAME_NUM_OF_YOUR_CHOICE --syn_mp MAPPOINT_NUM_OF_YOUR_CHOICE --budget BUDGET_OF_YOUR_CHOICE
```

## Reference

My implementation is based on the localisation approximation utility from:

> Müller, C., & Dayoub, F. (2022). *Map Point Selection for Visual SLAM*. IEEE International Conference on Robotics and Automation (ICRA). https://arxiv.org/abs/2209.09445