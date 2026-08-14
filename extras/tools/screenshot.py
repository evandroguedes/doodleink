#!/usr/bin/env python3
"""Capture a pixel-perfect screenshot from the Shopagotchi over USB serial.

The firmware dumps its framebuffer when it receives the letter "s"; this
script sends it, decodes the hex stream, and saves a PNG (scaled 3x with
crisp pixels so the art is easy to inspect).

Usage:
  python3 tools/screenshot.py                       # auto-detect port -> shot.png
  python3 tools/screenshot.py -o sick.png
  python3 tools/screenshot.py -n 4                  # shot-1.png ... shot-4.png
  python3 tools/screenshot.py -p /dev/cu.usbserial-XXXX

Requires:  pip install pyserial pillow
Close any open serial monitor first; only one program can hold the port.
"""

import argparse
import sys
import time

import serial
from serial.tools import list_ports


def find_port():
    for p in list_ports.comports():
        if any(k in p.device for k in ("usbserial", "wchusbserial", "usbmodem", "SLAB")):
            return p.device
    return None


def capture(ser):
    ser.reset_input_buffer()
    ser.write(b"s")
    header = None
    deadline = time.time() + 15
    while time.time() < deadline:
        line = ser.readline().decode(errors="replace").strip()
        if line.startswith("<<<SHOT"):
            header = line.split()
            break
    if not header:
        raise RuntimeError(
            "no screenshot header received; is the device flashed with the "
            "screenshot-enabled firmware, and the serial monitor closed?"
        )
    w, h = int(header[1]), int(header[2])
    pixels = []
    for _ in range(h):
        line = ser.readline().decode(errors="replace").strip()
        # Serial is lossy enough to occasionally drop a byte; a strict parse
        # would abort a whole 10s capture over a couple of pixels. Pad short
        # rows and render corrupt pixels black instead.
        if len(line) < w * 4:
            line = line + "0" * (w * 4 - len(line))
        for i in range(0, w * 4, 4):
            try:
                c = int(line[i : i + 4], 16)
            except ValueError:
                c = 0
            r = (c >> 11 & 0x1F) * 255 // 31
            g = (c >> 5 & 0x3F) * 255 // 63
            b = (c & 0x1F) * 255 // 31
            pixels.append((r, g, b))
    return w, h, pixels


def save(w, h, pixels, path):
    try:
        from PIL import Image
    except ImportError:
        path = path.rsplit(".", 1)[0] + ".ppm"
        with open(path, "wb") as f:
            f.write(f"P6 {w} {h} 255\n".encode())
            for px in pixels:
                f.write(bytes(px))
        print(f"Pillow not installed, wrote {path} (run: pip install pillow)")
        return
    img = Image.new("RGB", (w, h))
    img.putdata(pixels)
    img = img.resize((w * 3, h * 3), Image.NEAREST)
    img.save(path)
    print(f"saved {path}")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("-p", "--port", default=None)
    ap.add_argument("-o", "--out", default="shot.png")
    ap.add_argument("-n", "--count", type=int, default=1)
    ap.add_argument("--baud", type=int, default=115200)
    args = ap.parse_args()

    port = args.port or find_port()
    if not port:
        sys.exit("no serial port found; pass one with -p /dev/cu.usbserial-XXXX")

    ser = serial.Serial()
    ser.port = port
    ser.baudrate = args.baud
    ser.timeout = 5
    # keep DTR/RTS low so opening the port does not reset the ESP32
    ser.dtr = False
    ser.rts = False
    ser.open()
    time.sleep(0.3)

    try:
        for i in range(args.count):
            w, h, pixels = capture(ser)
            if args.count == 1:
                out = args.out
            else:
                stem, _, ext = args.out.rpartition(".")
                out = f"{stem or 'shot'}-{i + 1}.{ext or 'png'}"
            save(w, h, pixels, out)
    finally:
        ser.close()


if __name__ == "__main__":
    main()
