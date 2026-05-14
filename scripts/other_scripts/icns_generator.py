#!/usr/bin/env python3
import struct
import sys
from pathlib import Path

CHUNKS = [
    ("icp4", "icon_16x16.png"),
    ("icp5", "icon_32x32.png"),
    ("icp6", "icon_32x32@2x.png"),
    ("ic07", "icon_128x128.png"),
    ("ic08", "icon_256x256.png"),
    ("ic09", "icon_512x512.png"),
    ("ic10", "icon_512x512@2x.png"),
    ("ic11", "icon_16x16@2x.png"),
    ("ic12", "icon_32x32@2x.png"),
    ("ic13", "icon_128x128@2x.png"),
    ("ic14", "icon_256x256@2x.png"),
]

PNG_MAGIC = b"\x89PNG\r\n\x1a\n"

def make_chunk(chunk_type: str, png_path: Path) -> bytes:
    data = png_path.read_bytes()

    if not data.startswith(PNG_MAGIC):
        raise ValueError(f"{png_path} is not a PNG file")

    chunk_type_bytes = chunk_type.encode("ascii")
    chunk_length = 8 + len(data)

    return chunk_type_bytes + struct.pack(">I", chunk_length) + data

def main() -> int:
    if len(sys.argv) != 3:
        print("usage: make_icns_png_chunks.py <iconset-dir> <output.icns>", file=sys.stderr)
        return 2

    iconset_dir = Path(sys.argv[1])
    output_path = Path(sys.argv[2])

    chunks = []

    for chunk_type, filename in CHUNKS:
        png_path = iconset_dir / filename

        if png_path.exists():
            chunks.append(make_chunk(chunk_type, png_path))

    if not chunks:
        print(f"error: no PNG icon chunks found in {iconset_dir}", file=sys.stderr)
        return 1

    body = b"".join(chunks)
    header = b"icns" + struct.pack(">I", 8 + len(body))

    output_path.write_bytes(header + body)
    print(f"wrote {output_path} with {len(chunks)} PNG chunks")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())