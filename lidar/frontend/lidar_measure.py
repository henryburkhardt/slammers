from utils import load_and_filter_scan

s = load_and_filter_scan(8)

ranges = [p[0] for p in s]

print(max(ranges), min(ranges))