import tempfile
from pathlib import Path
from unittest import TestCase, skipUnless

import spiky_nes

from .rom_builder import DEMO_ROM, JAM_ROM

ROMS_DIR = Path(__file__).resolve().parents[2] / "tests" / "roms"
NESTEST_ROM = ROMS_DIR / "cpu" / "nestest.nes"
NESTEST_LOG = ROMS_DIR / "cpu" / "nestest.log"


class TestEmulator(TestCase):

    def setUp(self) -> None:
        self.emulator = spiky_nes.Emulator()
        self.assertTrue(self.emulator.load_rom_from_bytes(DEMO_ROM))

    def test_invalid_rom(self):
        emulator = spiky_nes.Emulator()
        self.assertFalse(emulator.load_rom_from_bytes(b"not a rom"))
        self.assertFalse(emulator.load_rom_from_bytes(DEMO_ROM[:100]))
        self.assertFalse(emulator.load_rom("/nonexistent/rom.nes"))

    def test_load_rom_from_file(self):
        with tempfile.TemporaryDirectory() as tmp_dir:
            path = Path(tmp_dir) / "demo.nes"
            path.write_bytes(DEMO_ROM)
            emulator = spiky_nes.Emulator()
            self.assertTrue(emulator.load_rom(str(path)))
        self.assertEqual(0, emulator.mapper())
        self.assertEqual(0x8000, emulator.cpu_registers.PC)

    def test_reset_vector(self):
        self.assertEqual(0x8000, self.emulator.cpu_registers.PC)
        self.assertEqual(0, self.emulator.mapper())

    def test_exec_op(self):
        self.assertEqual(2, self.emulator.exec_op())
        self.assertEqual(0x8001, self.emulator.cpu_registers.PC)
        for _ in range(5):
            self.emulator.exec_op()
        self.assertEqual(0x42, self.emulator.cpu_registers.A)
        self.assertEqual(0xFF, self.emulator.cpu_registers.SP)
        self.assertEqual(0x42, self.emulator.cpu_read(0x0000))

    def test_cpu_registers_writable(self):
        registers = self.emulator.cpu_registers
        registers.A = 0x12
        registers.PC = 0x8005
        self.assertEqual(0x12, self.emulator.cpu_registers.A)
        self.assertEqual(0x8005, self.emulator.cpu_registers.PC)

    def test_cpu_write(self):
        self.emulator.cpu_write(0x0123, 0xAB)
        self.assertEqual(0xAB, self.emulator.cpu_read(0x0123))
        self.assertEqual(0xAB, self.emulator.cpu_read(0x0923))

    def test_nmi_each_frame(self):
        self.emulator.run_frame()
        start = self.emulator.cpu_read(0x0001)
        for _ in range(10):
            self.emulator.run_frame()
        self.assertEqual(10, (self.emulator.cpu_read(0x0001) - start) & 0xFF)
        self.assertEqual(11, self.emulator.frame())

    def test_run_milliseconds(self):
        self.emulator.run(1000.0)
        self.assertAlmostEqual(1789773, self.emulator.cpu_cycles(), delta=10)
        self.assertIn(self.emulator.frame(), (59, 60))

    def test_controller(self):
        self.emulator.run_frame()
        self.assertEqual(0, self.emulator.cpu_read(0x0002))
        self.emulator.press(spiky_nes.Button.A)
        self.emulator.run_frame()
        self.assertEqual(1, self.emulator.cpu_read(0x0002))
        self.emulator.release(spiky_nes.Button.A)
        self.emulator.run_frame()
        self.assertEqual(0, self.emulator.cpu_read(0x0002))
        self.emulator.press(spiky_nes.Button.A, player=2)
        self.emulator.run_frame()
        self.assertEqual(0, self.emulator.cpu_read(0x0002))

    def test_frame_buffer(self):
        self.emulator.run_frame()
        self.emulator.run_frame()
        frame = self.emulator.frame_buffer()
        self.assertEqual(
            spiky_nes.SCREEN_WIDTH * spiky_nes.SCREEN_HEIGHT * 4, len(frame)
        )
        self.assertEqual(bytes([0x3C, 0xBC, 0xFC, 0xFF]) * (len(frame) // 4), frame)

    def test_audio_samples(self):
        samples = [self.emulator.run_frame(44100.0) for _ in range(60)]
        total = sum(len(s) for s in samples)
        self.assertAlmostEqual(44100 * 29780.5 / 1789773 * 60, total, delta=2)
        self.assertTrue(all(len(s) in (733, 734) for s in samples[1:]))
        self.assertTrue(all(isinstance(v, float) for v in samples[1]))
        self.assertAlmostEqual(0.0, self.emulator.audio_sample())

    def test_reset(self):
        for _ in range(3):
            self.emulator.run_frame()
        self.emulator.cpu_registers.PC = 0x1234
        self.emulator.reset()
        self.assertEqual(0x8000, self.emulator.cpu_registers.PC)

    def test_halted(self):
        emulator = spiky_nes.Emulator()
        self.assertTrue(emulator.load_rom_from_bytes(JAM_ROM))
        self.assertFalse(emulator.halted())
        emulator.exec_op()
        self.assertTrue(emulator.halted())
        self.assertEqual([], emulator.run_frame())

    @skipUnless(
        NESTEST_ROM.exists() and NESTEST_LOG.exists(), "nestest ROM not downloaded"
    )
    def test_nestest(self):
        emulator = spiky_nes.Emulator()
        self.assertTrue(emulator.load_rom(str(NESTEST_ROM)))
        emulator.cpu_registers.PC = 0xC000
        lines = [line for line in NESTEST_LOG.read_text().splitlines() if line]
        for i, line in enumerate(lines):
            self.assertEqual(
                int(line[:4], 16), emulator.cpu_registers.PC, f"instruction {i}: {line}"
            )
            emulator.exec_op()
        self.assertEqual(0, emulator.cpu_read(0x02))
        self.assertEqual(0, emulator.cpu_read(0x03))
