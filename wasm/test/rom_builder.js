const PRG_SIZE = 0x4000;
const CHR_SIZE = 0x2000;
const PRG_START = 0x8000;

function buildNROM(resetCode, nmiCode = [0x40]) {
    const rom = new Uint8Array(16 + PRG_SIZE + CHR_SIZE);
    rom.set([0x4E, 0x45, 0x53, 0x1A, 1, 1]);
    const prg = rom.subarray(16, 16 + PRG_SIZE);
    prg.set(resetCode);
    prg.set(nmiCode, resetCode.length);
    const nmiAddr = PRG_START + resetCode.length;
    [nmiAddr, PRG_START, nmiAddr].forEach((addr, i) => {
        prg[0x3FFA + i * 2] = addr & 0xFF;
        prg[0x3FFB + i * 2] = addr >> 8;
    });
    return rom;
}

const jmp = (addr) => [0x4C, addr & 0xFF, addr >> 8];

const INIT = [
    0x78,              // SEI
    0xD8,              // CLD
    0xA2, 0xFF,        // LDX #$FF
    0x9A,              // TXS
    0xA9, 0x42,        // LDA #$42
    0x85, 0x00,        // STA $00
    0xA9, 0x3F,        // LDA #$3F
    0x8D, 0x06, 0x20,  // STA $2006
    0xA9, 0x00,        // LDA #$00
    0x8D, 0x06, 0x20,  // STA $2006
    0xA9, 0x21,        // LDA #$21
    0x8D, 0x07, 0x20,  // STA $2007
    0xA9, 0x80,        // LDA #$80
    0x8D, 0x00, 0x20,  // STA $2000
];

const POLL_CONTROLLER = [
    0xA9, 0x01,        // LDA #$01
    0x8D, 0x16, 0x40,  // STA $4016
    0xA9, 0x00,        // LDA #$00
    0x8D, 0x16, 0x40,  // STA $4016
    0xAD, 0x16, 0x40,  // LDA $4016
    0x29, 0x01,        // AND #$01
    0x85, 0x02,        // STA $02
];

const INC_COUNTER_RTI = [0xE6, 0x01, 0x40];  // INC $01; RTI

export const DEMO_ROM = buildNROM([...INIT, ...POLL_CONTROLLER, ...jmp(PRG_START + INIT.length)], INC_COUNTER_RTI);

export const JAM_ROM = buildNROM([0x02]);
