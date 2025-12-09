import cv2
import numpy as np

WIDTH = 128
HEIGHT = 8   # 8 pages = 8 rows in your struct
PAGES = HEIGHT

def frame_to_ssd1306(img):
    """
    Convert a 128x64 grayscale image to SSD1306 page format.
    Returns a list of 8 rows, each row is 128 bytes.
    """
    # Resize image to 128x64
    img_resized = cv2.resize(img, (WIDTH, HEIGHT * 8), interpolation=cv2.INTER_AREA)
    # Threshold to 0 or 1
    binary = (img_resized > 127).astype(np.uint8)

    rows = []
    for page in range(PAGES):
        row_bytes = []
        y0 = page * 8
        for x in range(WIDTH):
            byte = 0
            for bit in range(8):
                byte |= (binary[y0 + bit, x] << bit)
            row_bytes.append(byte)
        rows.append(row_bytes)
    return rows

def convert_video_to_struct(video_path, out_h):
    cap = cv2.VideoCapture(video_path)
    frames = []

    while True:
        ret, frame = cap.read()
        if not ret:
            break
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        rows = frame_to_ssd1306(gray)
        frames.append(rows)

    cap.release()

    with open(out_h, "w") as f:
        f.write("// Auto-generated SSD1306 video frames as 'frame' structs\n")
        f.write("#pragma once\n#include <stdint.h>\n\n")
        f.write(f"#define VIDEO_FRAMES {len(frames)}\n\n")
        f.write("frame video[VIDEO_FRAMES] = {\n")

        for fi, rows in enumerate(frames):
            f.write(f"    {{ // frame {fi}\n        .rows = {{\n")
            for row in rows:
                f.write("            { { ")
                f.write(", ".join(f"0x{b:02X}" for b in row))
                f.write(" } },\n")
            f.write("        }\n    },\n")
        f.write("};\n")

# Example usage:
# convert_video_to_struct("input.mp4", "video.h")



# Example usage:
convert_video_to_struct("input.mp4", "frames.h")
