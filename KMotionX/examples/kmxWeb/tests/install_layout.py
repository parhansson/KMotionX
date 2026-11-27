"""Verify backend install ownership using only temporary destinations."""
import os
from pathlib import Path
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[4]
with tempfile.TemporaryDirectory(prefix="kmx-layout-", dir=os.environ.get("TMPDIR")) as temp:
    root = Path(temp)
    home = root / "home"
    prefix = root / "prefix with spaces"
    options = [
        f"HOME={home}", f"bindir={prefix}/bin", f"kmxdir={prefix}/lib/kmx",
        f"kmxlibexecdir={prefix}/libexec/kmx", f"kmxdatadir={prefix}/share/kmx",
        f"includedir={prefix}/include"
    ]

    def make(target, extra=()):
        subprocess.run(["make", target, *options, *extra], cwd=repo, check=True,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    make("install")
    for name in ("KMotionServer", "tcc67"):
        assert (prefix / "libexec/kmx" / name).is_file()
        assert not (prefix / "bin" / name).exists()
    assert (prefix / "bin/kmxWeb").is_file()
    assert list((prefix / "lib/kmx").glob("libKMotionX.*"))
    assert (prefix / "share/kmx/DSP_KFLOP/DSPKFLOP.out").is_file()
    assert (prefix / "share/kmx/DSP_KOGNA/DSPKOGNA.out").is_file()
    machine = home / ".kmotionx/default-machine"
    files = ("data/emc.var", "data/Default.tbl", "data/Kinematics.txt", "c-programs/BlinkKFLOP.c")
    for name in files:
        assert (machine / name).is_file()
        (machine / name).write_text("user modified")
    language = home / ".kmotionx/data/LocalLanguage.txt"
    language.parent.mkdir(parents=True)
    language.write_text("user language")
    frontend = prefix / "share/kmx/web/index.html"
    frontend.parent.mkdir(parents=True)
    frontend.write_text("frontend-owned")
    make("install")
    for name in files:
        assert (machine / name).read_text() == "user modified"
    make("uninstall")
    for name in files:
        assert (machine / name).read_text() == "user modified"
    assert language.read_text() == "user language"
    assert frontend.read_text() == "frontend-owned"
    assert not (prefix / "bin/kmxWeb").exists()
    assert not (prefix / "share/kmx/DSP_KFLOP").exists()
    # Explicit destinations above already point into the sandbox. DESTDIR here
    # exercises the makefile's user-machine seeding guard, not path expansion.
    staging_home = root / "staging-home"
    make("install", [f"HOME={staging_home}", f"DESTDIR={root}/stage"])
    assert not staging_home.exists()
    make("uninstall")
    print("Backend install/reinstall/uninstall preserve machine, language and frontend files")
