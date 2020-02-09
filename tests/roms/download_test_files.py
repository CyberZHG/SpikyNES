from pathlib import Path
import urllib.request

CURRENT_DIR = Path(__file__).parent

CPU_TEST_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/other/nestest.txt",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/other/nestest.nes",
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/other/nestest.log",
]

download_dir = CURRENT_DIR / "cpu"
download_dir.mkdir(parents=True, exist_ok=True)
for url in CPU_TEST_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)

PPU_TEST_FILE_URLS = [
    "https://raw.githubusercontent.com/christopherpow/nes-test-roms/refs/heads/master/ppu_vbl_nmi/ppu_vbl_nmi.nes",
]

download_dir = CURRENT_DIR / "ppu"
download_dir.mkdir(parents=True, exist_ok=True)
for url in PPU_TEST_FILE_URLS:
    filename = url.split("/")[-1]
    urllib.request.urlretrieve(url, download_dir / filename)
