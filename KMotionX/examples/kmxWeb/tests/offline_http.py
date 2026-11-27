"""Run against the built backend with an isolated HOME; no hardware access."""
import http.client
import json
import os
from pathlib import Path
import select
import shutil
import socket
import subprocess
import tempfile
import time

repo = Path(__file__).resolve().parents[4]
with tempfile.TemporaryDirectory(prefix="kmx-http-", dir=os.environ.get("TMPDIR")) as tmp:
    home = Path(tmp)
    machine = home / "laser machine"
    shutil.copytree(repo / "KMotionX/default-machine", machine)
    (machine / "settings/fonts").mkdir(parents=True)
    (machine / "settings/textures").mkdir()
    (machine / "gcode").mkdir()
    (machine / "settings/machine.json").write_text(json.dumps({
        "name": "preview", "axes": [{"name": axis} for axis in "XYZABC"],
        "tplanner": {}, "actions": [], "userActions": [],
        "specialActions": [], "extendedActions": []
    }))
    (machine / "settings/importcnf.json").write_text('{"svg": {}}')
    (machine / "settings/fonts/font.ttf").write_bytes(b"font fixture")
    (machine / "settings/textures/texture.png").write_bytes(b"texture fixture")
    (machine / "gcode/example.ngc").write_text("M2\n")
    (machine / "escape").symlink_to(home, target_is_directory=True)
    (home / ".kmxrc").write_text(f"machineDataPath={machine}\n")
    webroot = home / "web assets"
    webroot.mkdir()
    (webroot / "index.html").write_text("<html>frontend fixture</html>")
    (webroot / "app.js").write_text("// static asset")
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
    env = dict(os.environ, HOME=str(home), KMX_WEB_OFFLINE="1")
    process = subprocess.Popen([
        str(repo / "bin/kmxWeb"), "-document_root", str(webroot),
        "-listening_port", f"127.0.0.1:{port}"
    ], cwd=home, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    try:
        # Wait for the explicit startup line, not for hardware or arbitrary sleeps.
        log = b""
        deadline = time.monotonic() + 15
        while b" serving [" not in log:
            remaining = deadline - time.monotonic()
            assert remaining > 0, log.decode(errors="replace")
            readable, _, _ = select.select([process.stdout], [], [], remaining)
            assert readable, log.decode(errors="replace")
            chunk = os.read(process.stdout.fileno(), 65536)
            assert chunk, log.decode(errors="replace")
            log += chunk
        assert b"Launching KMotionServer" not in log, log.decode(errors="replace")

        def request(path, body=None, headers=None):
            connection = http.client.HTTPConnection("127.0.0.1", port, timeout=5)
            connection.request("GET" if body is None else "POST", path, body, headers or {})
            response = connection.getresponse()
            result = response.status, response.read()
            connection.close()
            return result

        def api(name, value):
            return request("/api/kmx/" + name, json.dumps({"params": value}),
                           {"Content-Type": "application/json"})

        for route in ["/", "/gcode", "/settings"]:
            status, content = request(route)
            assert status == 200 and b"frontend fixture" in content, (route, status, content)
        assert request("/app.js") == (200, b"// static asset")
        assert request("/settings/fonts/font.ttf") == (200, b"font fixture")
        assert request("/settings/textures/texture.png") == (200, b"texture fixture")
        assert request("/settings/machine.json")[0] == 404
        status, content = api("openFile", "settings/importcnf.json")
        assert status == 200 and json.loads(content) == {"svg": {}}, (status, content)
        status, content = api("listDir", "gcode")
        assert status == 200 and b"example.ngc" in content, (status, content)
        for path in ["../.kmxrc", str(home / ".kmxrc"), "escape/.kmxrc"]:
            assert api("openFile", path)[0] == 403, path
        assert api("setMachineFile", "other.json")[0] == 409
        assert api("onCycleStart", None)[0] == 403
        assert api("setGcodeFile", "gcode/example.ngc")[0] == 200

        def upload(filename, content="M2\n"):
            body = (f'--fixture\r\nContent-Disposition: form-data; name="file"; '
                    f'filename="{filename}"\r\nContent-Type: text/plain\r\n\r\n'
                    f'{content}\r\n--fixture--\r\n')
            return request("/upload", body, {
                "Content-Type": "multipart/form-data; boundary=fixture"
            })

        status, content = upload("gcode/new file.ngc")
        assert status == 200, (status, content)
        assert (machine / "gcode/new file.ngc").read_text() == "M2\n"
        assert upload("../escape.ngc")[0] == 403
        assert not (home / "escape.ngc").exists()
        assert upload("settings/importcnf.json", '{"svg":{"dpi":96}}')[0] == 200
        assert json.loads((machine / "settings/importcnf.json").read_text()) == {"svg": {"dpi": 96}}
        ws = socket.create_connection(("127.0.0.1", port), timeout=5)
        try:
            ws.sendall((
                f"GET /ws HTTP/1.1\r\nHost: localhost:{port}\r\n"
                "Upgrade: websocket\r\nConnection: Upgrade\r\n"
                "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                "Sec-WebSocket-Version: 13\r\n\r\n"
            ).encode())
            assert b"101" in ws.recv(4096)
        finally:
            ws.close()
        print("Offline HTTP tests passed: independent roots, resources, files, upload, isolation")
    finally:
        process.terminate()
        try:
            tail, _ = process.communicate(timeout=5)
            assert b"Launching KMotionServer" not in log + tail
        except subprocess.TimeoutExpired:
            process.kill()
            process.communicate()
