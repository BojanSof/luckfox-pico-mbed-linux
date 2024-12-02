import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def nv12_to_rgb(nv12, w, h):
    n = w * h
    y, u, v = nv12[:n], nv12[n + 0::2], nv12[n + 1::2]
    yuv = bytearray(3 * n)
    size = (w, h)
    yuv[0::3] = y
    yuv[1::3] = Image.frombytes('L', (w // 2, h // 2), u).resize(size).tobytes()
    yuv[2::3] = Image.frombytes('L', (w // 2, h // 2), v).resize(size).tobytes()
    return Image.frombuffer('YCbCr', size, yuv).convert('RGB')

nv12_file = "frame.raw"
width, height = 640, 480

with open(nv12_file, "rb") as f:
    nv12 = f.read()

rgb = nv12_to_rgb(nv12, width, height)
rgb.save("frame.png")

plt.imshow(rgb)
plt.axis("off")
plt.show()

