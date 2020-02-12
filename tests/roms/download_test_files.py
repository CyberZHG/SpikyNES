from pathlib import Path
import urllib.request

CURRENT_DIR = Path(__file__).parent

CPU_TEST_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/other/nestest.txt",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/other/nestest.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/other/nestest.log",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/cpu_timing_test6/cpu_timing_test.nes",
]

download_dir = CURRENT_DIR / "cpu"
download_dir.mkdir(parents=True, exist_ok=True)
for url in CPU_TEST_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)

PPU_TEST_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/ppu_vbl_nmi/ppu_vbl_nmi.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/blargg_ppu_tests_2005.09.15b/palette_ram.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/blargg_ppu_tests_2005.09.15b/power_up_palette.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/blargg_ppu_tests_2005.09.15b/sprite_ram.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/blargg_ppu_tests_2005.09.15b/vbl_clear_time.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/blargg_ppu_tests_2005.09.15b/vram_access.nes",
]

download_dir = CURRENT_DIR / "ppu"
download_dir.mkdir(parents=True, exist_ok=True)
for url in PPU_TEST_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)

PPU_SPRITE_HIT_TEST_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/01.basics.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/02.alignment.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/03.corners.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/04.flip.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/05.left_clip.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/06.right_edge.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/07.screen_bottom.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/08.double_height.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/09.timing_basics.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/10.timing_order.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_hit_tests_2005.10.05/11.edge_timing.nes",
]

download_dir = CURRENT_DIR / "ppu" / "sprite_hit_tests"
download_dir.mkdir(parents=True, exist_ok=True)
for url in PPU_SPRITE_HIT_TEST_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)

PPU_SPRITE_OVERFLOW_TEST_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_overflow_tests/1.Basics.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_overflow_tests/2.Details.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_overflow_tests/3.Timing.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_overflow_tests/4.Obscure.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/sprite_overflow_tests/5.Emulator.nes",
]
download_dir = CURRENT_DIR / "ppu" / "sprite_overflow_tests"
download_dir.mkdir(parents=True, exist_ok=True)
for url in PPU_SPRITE_OVERFLOW_TEST_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)

PPU_VBL_NMI_TIMING_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/1.frame_basics.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/2.vbl_timing.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/3.even_odd_frames.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/4.vbl_clear_timing.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/5.nmi_suppression.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/6.nmi_disable.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/vbl_nmi_timing/7.nmi_timing.nes",
]
download_dir = CURRENT_DIR / "ppu" / "vbl_nmi_timing"
download_dir.mkdir(parents=True, exist_ok=True)
for url in PPU_VBL_NMI_TIMING_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)