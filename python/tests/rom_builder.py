PRG_SIZE = 0x4000
CHR_SIZE = 0x2000
PRG_START = 0x8000


def build_nrom(reset_code: bytes, nmi_code: bytes = b"\x40") -> bytes:
    prg = bytearray(PRG_SIZE)
    prg[: len(reset_code)] = reset_code
    nmi_offset = len(reset_code)
    prg[nmi_offset : nmi_offset + len(nmi_code)] = nmi_code
    nmi_addr = PRG_START + nmi_offset
    vectors = [nmi_addr, PRG_START, nmi_addr]
    for i, addr in enumerate(vectors):
        prg[0x3FFA + i * 2] = addr & 0xFF
        prg[0x3FFB + i * 2] = addr >> 8
    header = b"NES\x1a" + bytes([1, 1, 0, 0]) + bytes(8)
    return header + bytes(prg) + bytes(CHR_SIZE)


def jmp(addr: int) -> bytes:
    return bytes([0x4C, addr & 0xFF, addr >> 8])


INIT = bytes(
    [
        0x78,  # SEI
        0xD8,  # CLD
        0xA2, 0xFF,  # LDX #$FF
        0x9A,  # TXS
        0xA9, 0x42,  # LDA #$42
        0x85, 0x00,  # STA $00
        0xA9, 0x3F,  # LDA #$3F
        0x8D, 0x06, 0x20,  # STA $2006
        0xA9, 0x00,  # LDA #$00
        0x8D, 0x06, 0x20,  # STA $2006
        0xA9, 0x21,  # LDA #$21
        0x8D, 0x07, 0x20,  # STA $2007
        0xA9, 0x80,  # LDA #$80
        0x8D, 0x00, 0x20,  # STA $2000
    ]
)  # fmt: skip

POLL_CONTROLLER = bytes(
    [
        0xA9, 0x01,  # LDA #$01
        0x8D, 0x16, 0x40,  # STA $4016
        0xA9, 0x00,  # LDA #$00
        0x8D, 0x16, 0x40,  # STA $4016
        0xAD, 0x16, 0x40,  # LDA $4016
        0x29, 0x01,  # AND #$01
        0x85, 0x02,  # STA $02
    ]
)  # fmt: skip

INC_COUNTER_RTI = bytes([0xE6, 0x01, 0x40])  # INC $01; RTI

DEMO_ROM = build_nrom(
    INIT + POLL_CONTROLLER + jmp(PRG_START + len(INIT)), INC_COUNTER_RTI
)

JAM_ROM = build_nrom(bytes([0x02]))
