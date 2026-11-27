"""Inspect configure-generated Linux link commands on any host (not a build)."""
import os
from pathlib import Path
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[4]
with tempfile.TemporaryDirectory(prefix="kmx-linux-", dir=os.environ.get("TMPDIR")) as temp:
    root = Path(temp)
    tools = root / "tools"
    tools.mkdir()
    # Controlled target/dependency discovery; no host Homebrew flags in the result.
    (tools / "uname").write_text('#!/bin/sh\ncase "$1" in -m) echo x86_64;; *) echo Linux;; esac\n')
    (tools / "pkg-config").write_text(
        '#!/bin/sh\ncase "$1" in --exists|--cflags) exit 0;; --libs) echo "-lftdi1 -lusb-1.0";; esac\n')
    for tool in tools.iterdir():
        tool.chmod(0o755)
    env = dict(os.environ, HOME=str(root / "home"), PATH=f"{tools}:{os.environ['PATH']}")
    subprocess.run(["sh", str(repo / "configure"), "--prefix=/opt/kmx-preview"],
                   cwd=root, env=env, check=True, stdout=subprocess.PIPE)
    config = (root / "config.mak").read_text()
    assert "OSNAME=Linux" in config and "LIBEXT=.so" in config
    (root / "sample.cpp").write_text("int sample() { return 0; }\n")
    for library in (False, True):
        (root / "makefile").write_text(
            f"BUILD_ROOT={root}\ninclude config.mak\nSOURCES=sample.cpp\n"
            f"EXECUTABLE={'libprobe.so' if library else 'probe'}\n"
            f"BIN_TYPE={'.so' if library else ''}\n"
            f"include {repo}/include.mk\n")
        output = subprocess.check_output(["make", "-n", "DESTDIR=/staging"], cwd=root, env=env, text=True)
        assert "-rpath,'$ORIGIN'" in output, output
        assert '-rpath,"/opt/kmx-preview/lib/kmx"' in output, output
        assert "/staging" not in output, output
        assert ("-shared" in output) == library, output
        assert "@loader_path" not in output and "-arch arm64" not in output
    print("Linux configure/linker dry-run passed (executable and shared library)")
