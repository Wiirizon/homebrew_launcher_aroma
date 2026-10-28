// SPDX-License-Identifier: GPL-3.0-or-later
// Independently check the generated RPX header and WUHB /code/root.rpx entry.
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
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
for (const binary of [rpx, bundle]) {
    const strings = binary.toString('latin1');
    assert(!/[A-Za-z]:[\\/]Users[\\/]|\/home\/|Cafe_SDK/i.test(strings), 'No local paths or proprietary SDK references');
}
console.log('RPX header and WUHB embedded executable verified');
