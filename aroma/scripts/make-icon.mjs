// SPDX-License-Identifier: GPL-3.0-or-later
// Generate aroma/assets/icon.png (128x128 RGBA) from simple geometry.
// Stdlib only; no text chunks, so the output carries no author, time or path data.
import { deflateSync } from 'node:zlib';
import { writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const SIZE = 128;
const BACKGROUND = [0x14, 0x21, 0x3d];
const DISC = [0x2a, 0x9d, 0x8f];
const MARK = [0xff, 0xff, 0xff];

const CRC_TABLE = Array.from({ length: 256 }, (_, n) => {
    let c = n;
    for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    return c >>> 0;
});

function crc32(buffer) {
    let crc = 0xffffffff;
    for (const byte of buffer) crc = CRC_TABLE[(crc ^ byte) & 0xff] ^ (crc >>> 8);
    return (crc ^ 0xffffffff) >>> 0;
}

function chunk(type, data) {
    const header = Buffer.alloc(8);
    header.writeUInt32BE(data.length, 0);
    header.write(type, 4, 'latin1');
    const crc = Buffer.alloc(4);
    crc.writeUInt32BE(crc32(Buffer.concat([header.subarray(4), data])), 0);
    return Buffer.concat([header, data, crc]);
}

function encodePng(width, height, rgba) {
    const ihdr = Buffer.alloc(13);
    ihdr.writeUInt32BE(width, 0);
    ihdr.writeUInt32BE(height, 4);
    ihdr[8] = 8; // bit depth
    ihdr[9] = 6; // RGBA
    const stride = width * 4;
    const raw = Buffer.alloc((stride + 1) * height);
    for (let y = 0; y < height; y++) {
        raw[y * (stride + 1)] = 0; // filter: none
        rgba.copy(raw, y * (stride + 1) + 1, y * stride, (y + 1) * stride);
    }
    return Buffer.concat([
        Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]),
        chunk('IHDR', ihdr),
        chunk('IDAT', deflateSync(raw, { level: 9 })),
        chunk('IEND', Buffer.alloc(0)),
    ]);
}

// Right-pointing triangle centred in the disc (launch symbol).
function insideMark(x, y) {
    const left = 50, right = 84, top = 40, bottom = 88;
    if (x < left || x > right || y < top || y > bottom) return false;
    const halfHeightAtX = (1 - (x - left) / (right - left)) * (bottom - top) / 2;
    return Math.abs(y - (top + bottom) / 2) <= halfHeightAtX;
}

function insideDisc(x, y) {
    const dx = x + 0.5 - SIZE / 2, dy = y + 0.5 - SIZE / 2;
    return dx * dx + dy * dy <= 52 * 52;
}

function drawIcon() {
    const pixels = Buffer.alloc(SIZE * SIZE * 4);
    for (let y = 0; y < SIZE; y++) {
        for (let x = 0; x < SIZE; x++) {
            let colour = BACKGROUND;
            if (insideDisc(x, y)) colour = DISC;
            if (insideDisc(x, y) && insideMark(x, y)) colour = MARK;
            const offset = (y * SIZE + x) * 4;
            pixels.set([...colour, 0xff], offset);
        }
    }
    return pixels;
}

const output = fileURLToPath(new URL('../assets/icon.png', import.meta.url));
writeFileSync(output, encodePng(SIZE, SIZE, drawIcon()));
console.log(`Wrote ${output}`);
