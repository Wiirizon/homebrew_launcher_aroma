// SPDX-License-Identifier: GPL-3.0-or-later
// Check the native app icon: PNG container, 128x128 size, safe metadata, and Makefile wiring.
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

const root = new URL('../../', import.meta.url);
const png = readFileSync(new URL('aroma/assets/icon.png', root));
const makefile = readFileSync(new URL('aroma/Makefile', root), 'utf8');

assert.equal(png.subarray(0, 8).toString('hex'), '89504e470d0a1a0a', 'PNG signature');

const chunks = [];
for (let pos = 8; pos + 12 <= png.length;) {
    const length = png.readUInt32BE(pos);
    const type = png.subarray(pos + 4, pos + 8).toString('latin1');
    assert(pos + 12 + length <= png.length, `chunk ${type} overruns file`);
    chunks.push({ type, data: png.subarray(pos + 8, pos + 8 + length) });
    pos += 12 + length;
}

assert.equal(chunks[0]?.type, 'IHDR', 'IHDR must come first');
assert.equal(chunks.at(-1)?.type, 'IEND', 'IEND must come last');

const ihdr = chunks[0].data;
assert.equal(ihdr.readUInt32BE(0), 128, 'width');
assert.equal(ihdr.readUInt32BE(4), 128, 'height');
assert.equal(ihdr[8], 8, 'bit depth');
assert([2, 6].includes(ihdr[9]), 'colour type must be RGB (2) or RGBA (6)');
assert.equal(ihdr[12], 0, 'no interlace');

const forbidden = ['tEXt', 'zTXt', 'iTXt', 'eXIf', 'tIME'];
for (const { type } of chunks) {
    assert(!forbidden.includes(type), `metadata chunk ${type} leaks author or machine data`);
}
assert(chunks.some(({ type }) => type === 'IDAT'), 'IDAT pixel data present');

const makefileLine = makefile.split('\n').find(line => /^APP_ICON\s*:?=/.test(line));
assert(makefileLine, 'Makefile sets APP_ICON');
assert.match(makefileLine, /assets\/icon\.png\s*$/, 'APP_ICON points at assets/icon.png');
assert(!makefileLine.includes('$(CURDIR)'), 'APP_ICON must be relative (spaces in paths)');
const includeLine = makefile.indexOf('include $(DEVKITPRO)/wut/share/wut_rules');
assert(includeLine > makefile.indexOf(makefileLine), 'APP_ICON must be set before wut_rules is included');

console.log('App icon tests passed');
