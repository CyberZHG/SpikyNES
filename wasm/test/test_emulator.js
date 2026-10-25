import { describe, it, beforeEach, afterEach } from 'mocha';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { SCREEN_WIDTH, SCREEN_HEIGHT, Button, Emulator } from '../index.js';
import { DEMO_ROM, JAM_ROM } from './rom_builder.js';

const ROMS_DIR = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '../../tests/roms');
const NESTEST_ROM = path.join(ROMS_DIR, 'cpu/nestest.nes');
const NESTEST_LOG = path.join(ROMS_DIR, 'cpu/nestest.log');

describe('Emulator', () => {
    let emulator;

    beforeEach(() => {
        emulator = new Emulator();
        assert.ok(emulator.loadROM(DEMO_ROM));
    });

    afterEach(() => {
        emulator.delete();
    });

    it('rejects invalid ROMs', () => {
        const other = new Emulator();
        assert.equal(other.loadROM(new TextEncoder().encode('not a rom')), false);
        assert.equal(other.loadROM(DEMO_ROM.subarray(0, 100)), false);
        other.delete();
    });

    it('accepts plain arrays', () => {
        const other = new Emulator();
        assert.ok(other.loadROM(Array.from(DEMO_ROM)));
        assert.equal(other.cpuRegisters().PC, 0x8000);
        other.delete();
    });

    it('starts at the reset vector', () => {
        assert.equal(emulator.cpuRegisters().PC, 0x8000);
        assert.equal(emulator.mapper(), 0);
    });

    it('executes instructions', () => {
        assert.equal(emulator.execOp(), 2);
        assert.equal(emulator.cpuRegisters().PC, 0x8001);
        for (let i = 0; i < 5; ++i) {
            emulator.execOp();
        }
        assert.equal(emulator.cpuRegisters().A, 0x42);
        assert.equal(emulator.cpuRegisters().SP, 0xFF);
        assert.equal(emulator.cpuRead(0x0000), 0x42);
    });

    it('writes CPU registers', () => {
        const registers = emulator.cpuRegisters();
        registers.A = 0x12;
        registers.PC = 0x8005;
        assert.equal(emulator.cpuRegisters().A, 0x12);
        assert.equal(emulator.cpuRegisters().PC, 0x8005);
    });

    it('writes CPU bus', () => {
        emulator.cpuWrite(0x0123, 0xAB);
        assert.equal(emulator.cpuRead(0x0123), 0xAB);
        assert.equal(emulator.cpuRead(0x0923), 0xAB);
    });

    it('triggers NMI each frame', () => {
        emulator.runFrame();
        const start = emulator.cpuRead(0x0001);
        for (let i = 0; i < 10; ++i) {
            emulator.runFrame();
        }
        assert.equal((emulator.cpuRead(0x0001) - start) & 0xFF, 10);
        assert.equal(emulator.frame(), 11);
    });

    it('runs for given milliseconds', () => {
        emulator.run(1000.0);
        assert.ok(Math.abs(emulator.cpuCycles() - 1789773) <= 10);
        assert.ok([59, 60].includes(emulator.frame()));
    });

    it('reads controller input', () => {
        emulator.runFrame();
        assert.equal(emulator.cpuRead(0x0002), 0);
        emulator.press(Button.A);
        emulator.runFrame();
        assert.equal(emulator.cpuRead(0x0002), 1);
        emulator.release(Button.A);
        emulator.runFrame();
        assert.equal(emulator.cpuRead(0x0002), 0);
        emulator.press(Button.A, 2);
        emulator.runFrame();
        assert.equal(emulator.cpuRead(0x0002), 0);
    });

    it('renders the backdrop color', () => {
        emulator.runFrame();
        emulator.runFrame();
        const frame = emulator.frameBuffer();
        assert.ok(frame instanceof Uint8ClampedArray);
        assert.equal(frame.length, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
        for (let i = 0; i < frame.length; i += 4) {
            assert.deepEqual(Array.from(frame.subarray(i, i + 4)), [0x3C, 0xBC, 0xFC, 0xFF]);
        }
    });

    it('produces audio samples', () => {
        const samples = [];
        for (let i = 0; i < 60; ++i) {
            samples.push(emulator.runFrame(44100));
        }
        const total = samples.reduce((sum, s) => sum + s.length, 0);
        assert.ok(Math.abs(total - 44100 * 29780.5 / 1789773 * 60) <= 2);
        for (const s of samples.slice(1)) {
            assert.ok(s instanceof Float32Array);
            assert.ok([733, 734].includes(s.length));
        }
        assert.equal(emulator.audioSample(), 0);
    });

    it('resets', () => {
        for (let i = 0; i < 3; ++i) {
            emulator.runFrame();
        }
        emulator.cpuRegisters().PC = 0x1234;
        emulator.reset();
        assert.equal(emulator.cpuRegisters().PC, 0x8000);
    });

    it('halts on JAM', () => {
        const other = new Emulator();
        assert.ok(other.loadROM(JAM_ROM));
        assert.equal(other.halted(), false);
        other.execOp();
        assert.equal(other.halted(), true);
        assert.equal(other.runFrame().length, 0);
        other.delete();
    });

    it('passes nestest', function () {
        if (!fs.existsSync(NESTEST_ROM) || !fs.existsSync(NESTEST_LOG)) {
            this.skip();
        }
        const other = new Emulator();
        assert.ok(other.loadROM(fs.readFileSync(NESTEST_ROM)));
        other.cpuRegisters().PC = 0xC000;
        const lines = fs.readFileSync(NESTEST_LOG, 'utf8').split('\n').filter((line) => line.length > 0);
        for (const [i, line] of lines.entries()) {
            assert.equal(other.cpuRegisters().PC, parseInt(line.substring(0, 4), 16), `instruction ${i}: ${line}`);
            other.execOp();
        }
        assert.equal(other.cpuRead(0x02), 0);
        assert.equal(other.cpuRead(0x03), 0);
        other.delete();
    });
});
