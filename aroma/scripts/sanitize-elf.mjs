// SPDX-License-Identifier: GPL-3.0-or-later
// Remove toolchain assertion-source paths before RPX compression. Only known
// newlib source strings in non-executable, allocated PROGBITS are changed.
import assert from 'node:assert/strict';
import { readFileSync, writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

export function sanitizeElf(input) {
    const output = Buffer.from(input);
    assert.equal(output.subarray(0, 7).toString('hex'), '7f454c46010201');
    assert.equal(output.readUInt16BE(18), 20);
    const table = output.readUInt32BE(32);
    const stride = output.readUInt16BE(46), count = output.readUInt16BE(48);
    assert(stride >= 40 && table + stride * count <= output.length);
    let replacements = 0;
    for (let i = 0; i < count; ++i) {
        const pos = table + i * stride;
        const type = output.readUInt32BE(pos + 4), flags = output.readUInt32BE(pos + 8);
        if (type !== 1 || !(flags & 2) || (flags & 4)) continue;
        const offset = output.readUInt32BE(pos + 16), size = output.readUInt32BE(pos + 20);
        assert(offset + size <= output.length);
        const section = output.subarray(offset, offset + size);
        const text = section.toString('latin1');
        // Keep string lengths, section offsets, relocation targets, and code
        // unchanged. The basename remains at the same byte position.
        const pattern = /\/home\/[^/\x00]+\/projects\/devkitpro\/pacman-packages\/devkitppc\/newlib\/src\/newlib-[^/\x00]+\/newlib\/libc\/stdlib\/(dtoa|mprec)\.c(?=\x00)/g;
        for (const match of text.matchAll(pattern)) {
            const suffix = `/newlib/libc/stdlib/${match[1]}.c`;
            const replacement = 'toolchain'.padEnd(match[0].length - suffix.length, '_') + suffix;
            assert.equal(replacement.length, match[0].length);
            section.write(replacement, match.index, 'latin1');
            ++replacements;
        }
    }
    return { output, replacements };
}

if (process.argv[1] && fileURLToPath(import.meta.url) === process.argv[1]) {
    assert.equal(process.argv.length, 3, 'Usage: node sanitize-elf.mjs <linked-elf>');
    const { output, replacements } = sanitizeElf(readFileSync(process.argv[2]));
    writeFileSync(process.argv[2], output);
    console.log(`Sanitized ${replacements} toolchain assertion paths`);
}
