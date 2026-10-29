// SPDX-License-Identifier: GPL-3.0-or-later
import assert from 'node:assert/strict';
import { sanitizeElf } from '../scripts/sanitize-elf.mjs';
const fixture = Buffer.alloc(2048);
Buffer.from('7f454c46010201', 'hex').copy(fixture);
fixture.writeUInt16BE(20, 18);
fixture.writeUInt32BE(64, 32);
fixture.writeUInt16BE(40, 46);
fixture.writeUInt16BE(3, 48);
const sample = '/home/BUILD_PLACEHOLDER/projects/devkitpro/pacman-packages/devkitppc/newlib/src/newlib-version/newlib/libc/stdlib/dtoa.c';
function section(index, flags, offset) {
    const pos = 64 + index * 40;
    fixture.writeUInt32BE(1, pos + 4);
    fixture.writeUInt32BE(flags, pos + 8);
    fixture.writeUInt32BE(offset, pos + 16);
    fixture.writeUInt32BE(400, pos + 20);
    fixture.write(sample, offset);
}
section(1, 2, 256); // Allocated read-only data.
section(2, 6, 768); // Executable data must never change.
const result = sanitizeElf(fixture);
assert.equal(result.replacements, 1);
assert.equal(result.output.length, fixture.length);
assert(!result.output.subarray(256, 656).toString().includes('/home/'));
assert.deepEqual(result.output.subarray(768, 1168), fixture.subarray(768, 1168));
assert.deepEqual(result.output.subarray(0, 256), fixture.subarray(0, 256));
assert.equal(sanitizeElf(result.output).replacements, 0);
assert.throws(() => sanitizeElf(Buffer.alloc(8)));
console.log('ELF path sanitization tests passed');
