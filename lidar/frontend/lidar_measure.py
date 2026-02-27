from utils import load_and_filter_scan

s = load_and_filter_scan(8)

angles = [p[1] for p in s]

print(max(angles), min(angles))