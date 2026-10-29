// SPDX-License-Identifier: GPL-3.0-or-later
// Independently check the generated RPX header and WUHB /code/root.rpx entry.
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { inflateSync } from 'node:zlib';
const base = new URL('../', import.meta.url);
const rpx = readFileSync(new URL('homebrew_launcher_aroma.rpx', base));
const bundle = readFileSync(new URL('homebrew_launcher_aroma.wuhb', base));
assert.equal(rpx.subarray(0, 7).toString('hex'), '7f454c46010201');
assert.equal(rpx.readUInt16BE(16), 0xfe01, 'Native RPX type');
assert.equal(rpx.readUInt16BE(18), 20, 'PowerPC machine');
assert.equal(bundle.subarray(0, 4).toString(), 'WUHB');
assert.equal(bundle.readUInt32BE(4), 0x50);
const value64 = offset => {
    const value = bundle.readBigUInt64BE(offset);
    assert(value <= BigInt(Number.MAX_SAFE_INTEGER));
    return Number(value);
};
const dirs = value64(0x18), dirSize = value64(0x20);
const files = value64(0x38), fileSize = value64(0x40);
const data = value64(0x48);
assert(dirs + dirSize <= bundle.length);
assert(files + fileSize <= bundle.length);
assert(data < bundle.length);
let codeOffset;
for (let offset = 0; offset < dirSize;) {
    const pos = dirs + offset;
    assert(offset + 24 <= dirSize);
    const length = bundle.readUInt32BE(pos + 20);
    assert(offset + 24 + length <= dirSize);
    const name = bundle.subarray(pos + 24, pos + 24 + length).toString();
    if (name === 'code' && bundle.readUInt32BE(pos) === 0) codeOffset = offset;
    offset += Math.ceil((24 + length) / 4) * 4;
}
assert.notEqual(codeOffset, undefined, 'Root code directory');
let found = false;
for (let offset = 0; offset < fileSize;) {
    const pos = files + offset;
    assert(offset + 32 <= fileSize);
    const length = bundle.readUInt32BE(pos + 28);
    assert(offset + 32 + length <= fileSize);
    const name = bundle.subarray(pos + 32, pos + 32 + length).toString();
    if (name === 'root.rpx' && bundle.readUInt32BE(pos) === codeOffset) {
        const start = data + value64(pos + 8), size = value64(pos + 16);
        assert(start + size <= bundle.length);
        assert.deepEqual(bundle.subarray(start, start + size), rpx);
        found = true;
    }
    offset += Math.ceil((32 + length) / 4) * 4;
}
assert(found, 'Bundled /code/root.rpx must match the built RPX');
// RPX strings may be hidden inside compressed sections. Inspect those too.
const binaries = [rpx, bundle];
const sectionTable = rpx.readUInt32BE(32);
const sectionStride = rpx.readUInt16BE(46), sectionCount = rpx.readUInt16BE(48);
assert(sectionStride >= 40);
assert(sectionTable + sectionStride * sectionCount <= rpx.length);
for (let i = 0; i < sectionCount; ++i) {
    const pos = sectionTable + i * sectionStride;
    const type = rpx.readUInt32BE(pos + 4), flags = rpx.readUInt32BE(pos + 8);
    const offset = rpx.readUInt32BE(pos + 16), size = rpx.readUInt32BE(pos + 20);
    if (type === 8 || size === 0) continue; // NOBITS sections have no file data.
    assert(offset + size <= rpx.length);
    let section = rpx.subarray(offset, offset + size);
    if (flags & 0x08000000) {
        const length = section.readUInt32BE(0);
        section = inflateSync(section.subarray(4), { maxOutputLength: 64 * 1024 * 1024 });
        assert.equal(section.length, length);
    }
    binaries.push(section);
}
for (const binary of binaries) {
    // Some public devkitPro newlib archives contain their maintainer's build
    // path in assertion messages. These two known library source paths are
    // toolchain provenance, not files from the user's build workspace.
    const strings = binary.toString('latin1').replace(
        /\/home\/[^/\x00]+\/projects\/devkitpro\/pacman-packages\/devkitppc\/newlib\/src\/newlib-[^/\x00]+\/newlib\/libc\/stdlib\/(?:dtoa|mprec)\.c/g,
        '<public devkitPro newlib source>');
    assert(!/[A-Za-z]:[\\/]Users[\\/]|\/home\/|Cafe_SDK/i.test(strings), 'No local paths or proprietary SDK references');
}
console.log('RPX header and WUHB embedded executable verified');
