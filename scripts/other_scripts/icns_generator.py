#!/usr/bin/env python3
import argparse
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

PNG_MAGIC = b"\x89PNG\r\n\x1a\n"

CHUNKS = []

USE_LEGACY_TINY_PNG_CHUNKS = False

if USE_LEGACY_TINY_PNG_CHUNKS:
    CHUNKS += [
        ("icp4", "icon_16x16.png", 16, True),
        ("icp5", "icon_32x32.png", 32, True),
    ]

CHUNKS += [
    ("ic11", "icon_16x16@2x.png", 32, True),
    ("ic12", "icon_32x32@2x.png", 64, True),
    ("ic07", "icon_128x128.png", 128, True),
    ("ic13", "icon_128x128@2x.png", 256, True),
    ("ic08", "icon_256x256.png", 256, True),
    ("ic14", "icon_256x256@2x.png", 512, True),
    ("ic09", "icon_512x512.png", 512, True),
    ("ic10", "icon_512x512@2x.png", 1024, True),
]

# PNG color types:
# 0 grayscale
# 2 truecolor RGB
# 3 indexed/palette
# 4 grayscale + alpha
# 6 truecolor RGBA
PNG_COLOR_TYPES = {
    0: "grayscale",
    2: "rgb",
    3: "indexed",
    4: "grayscale+alpha",
    6: "rgba",
}


def read_png_ihdr(path: Path):
    data = path.read_bytes()

    if not data.startswith(PNG_MAGIC):
        raise ValueError(f"{path} is not a PNG file")

    # PNG signature is 8 bytes.
    # First chunk should be IHDR:
    # length: 4 bytes, type: 4 bytes, data: 13 bytes, crc: 4 bytes
    if len(data) < 33:
        raise ValueError(f"{path} is too small to be a valid PNG")

    ihdr_len = struct.unpack(">I", data[8:12])[0]
    ihdr_type = data[12:16]

    if ihdr_type != b"IHDR" or ihdr_len != 13:
        raise ValueError(f"{path} has invalid/missing IHDR chunk")

    width, height = struct.unpack(">II", data[16:24])
    bit_depth = data[24]
    color_type = data[25]
    compression = data[26]
    filter_method = data[27]
    interlace = data[28]

    return {
        "width": width,
        "height": height,
        "bit_depth": bit_depth,
        "color_type": color_type,
        "compression": compression,
        "filter_method": filter_method,
        "interlace": interlace,
        "data": data,
    }


def validate_png(path: Path, expected_size: int, *, strict_rgba: bool):
    info = read_png_ihdr(path)

    width = info["width"]
    height = info["height"]
    bit_depth = info["bit_depth"]
    color_type = info["color_type"]
    interlace = info["interlace"]

    if width != expected_size or height != expected_size:
        raise ValueError(
            f"{path} has wrong dimensions: {width}x{height}, "
            f"expected {expected_size}x{expected_size}"
        )

    if bit_depth != 8:
        raise ValueError(
            f"{path} has unsupported bit depth {bit_depth}; expected 8"
        )

    if interlace != 0:
        raise ValueError(
            f"{path} is interlaced; expected non-interlaced PNG"
        )

    if strict_rgba and color_type != 6:
        readable = PNG_COLOR_TYPES.get(color_type, f"unknown({color_type})")
        raise ValueError(
            f"{path} has color type {readable}; expected rgba"
        )

    return info


def normalize_png_with_magick(src: Path, dst: Path, expected_size: int):
    magick = shutil.which("magick")
    if not magick:
        raise RuntimeError(
            "normalization requested but ImageMagick 'magick' was not found"
        )

    subprocess.run(
        [
            magick,
            str(src),
            "-resize", f"{expected_size}x{expected_size}!",
            "-alpha", "on",
            "-background", "none",
            "-depth", "8",
            "-define", "png:color-type=6",
            "-define", "png:bit-depth=8",
            "-define", "png:compression-filter=5",
            "-define", "png:compression-level=9",
            "-strip",
            str(dst),
        ],
        check=True,
    )


def make_chunk(chunk_type: str, png_path: Path, expected_size: int, *, strict_rgba: bool) -> bytes:
    info = validate_png(png_path, expected_size, strict_rgba=strict_rgba)
    data = info["data"]

    chunk_type_bytes = chunk_type.encode("ascii")
    if len(chunk_type_bytes) != 4:
        raise ValueError(f"invalid ICNS chunk type: {chunk_type!r}")

    chunk_length = 8 + len(data)
    return chunk_type_bytes + struct.pack(">I", chunk_length) + data


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Create .icns from a macOS .iconset using PNG ICNS chunks."
    )
    parser.add_argument("iconset_dir", type=Path)
    parser.add_argument("output_icns", type=Path)
    parser.add_argument(
        "--normalize",
        action="store_true",
        help="rewrite PNGs as clean 8-bit RGBA PNGs before packing",
    )
    parser.add_argument(
        "--no-strict-rgba",
        action="store_true",
        help="allow non-RGBA PNGs after validation",
    )
    args = parser.parse_args()

    iconset_dir: Path = args.iconset_dir
    output_path: Path = args.output_icns

    if not iconset_dir.is_dir():
        print(f"error: iconset dir does not exist: {iconset_dir}", file=sys.stderr)
        return 1

    strict_rgba = not args.no_strict_rgba
    chunks = []

    with tempfile.TemporaryDirectory(prefix="icns-pack-") as tmp:
        tmp_dir = Path(tmp)

        for chunk_type, filename, expected_size, required in CHUNKS:
            src = iconset_dir / filename

            if not src.exists():
                if required:
                    print(f"error: missing required icon: {src}", file=sys.stderr)
                    return 1
                continue

            png_path = src

            if args.normalize:
                normalized = tmp_dir / filename
                normalize_png_with_magick(src, normalized, expected_size)
                png_path = normalized

            try:
                info = validate_png(png_path, expected_size, strict_rgba=strict_rgba)
            except Exception as exc:
                print(f"error: {exc}", file=sys.stderr)
                return 1

            color_name = PNG_COLOR_TYPES.get(
                info["color_type"],
                f"unknown({info['color_type']})"
            )

            print(
                f"adding {chunk_type} {filename}: "
                f"{info['width']}x{info['height']} "
                f"depth={info['bit_depth']} "
                f"color={color_name}"
            )

            try:
                chunks.append(
                    make_chunk(
                        chunk_type,
                        png_path,
                        expected_size,
                        strict_rgba=strict_rgba,
                    )
                )
            except Exception as exc:
                print(f"error: {exc}", file=sys.stderr)
                return 1

    if not chunks:
        print(f"error: no PNG icon chunks found in {iconset_dir}", file=sys.stderr)
        return 1

    body = b"".join(chunks)
    header = b"icns" + struct.pack(">I", 8 + len(body))

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(header + body)

    print(f"wrote {output_path} with {len(chunks)} PNG chunks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())