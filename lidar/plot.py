import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

# 20x20 grid over a wide enough range so edges are ~0 (nearly white)
n = 20
x = np.linspace(-4, 4, n)
y = np.linspace(-4, 4, n)
X, Y = np.meshgrid(x, y)

# Standard 2D normal pdf (mean 0,0 and covariance I)
Z = (1 / (2 * np.pi)) * np.exp(-0.2 * (X**2 + Y**2))

# White -> Blue colormap
white_to_blue = LinearSegmentedColormap.from_list("white_to_blue", ["white", "red"])

plt.figure(figsize=(6, 5))
plt.imshow(
    Z,
    origin="lower",
    cmap=white_to_blue,
    extent=[x.min(), x.max(), y.min(), y.max()],
    interpolation="nearest",  # keep the 20x20 grid cells crisp
    aspect="equal",
)

plt.colorbar(label="Probability density")
plt.title("Standard 2D Normal (20×20 grid)")
plt.xlabel("x")
plt.ylabel("y")
plt.tight_layout()
plt.show()