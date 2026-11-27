# KMotionX

KMotionX is a Linux and macOS port of Dynomotion's KMotion libraries and
controller tools, based on **KMotion 5.5.1**.

For the CNC application, see [KMotionXCNC](https://github.com/parhansson/KMotionXCNC).

## Components

| Component | Purpose |
|-----------|---------|
| `libKMotionX` | Platform compatibility and runtime support |
| `libKMotion` | Controller communication and program loading |
| `libGCodeInterpreter` | G-code interpretation, coordinate motion and kinematics |
| `KMotionServer` | Controller communication server |
| `tcc67` | Bundled compiler for controller C programs |

The repository also includes examples and the `kmxWeb` executable.

### API compatibility

The 5.5.1 port uses wide-character filenames and callbacks (`wchar_t`) in the
native library API. Applications written against earlier KMotionX versions
may need changes to their calls and callback signatures.

## Build and install

### 1. Install prerequisites

You need Git, a C++20-capable compiler, Make, and the USB library development
packages for your platform. Follow the relevant platform guide:

- [macOS](KMotionX/doc/MacOSX.md)
- [Ubuntu](KMotionX/doc/Ubuntu.md)
- [Raspberry Pi](KMotionX/doc/RaspberryPi.md)

The default build uses `pkg-config` (or `pkgconf`) to locate `libftdi1` and
`libusb-1.0`. Check that both are available:

```sh
pkg-config --modversion libftdi1 libusb-1.0
```

If these dependencies are installed under a custom prefix, expose their
`.pc` files before configuring:

```sh
export PKG_CONFIG_PATH="/path/to/dependencies/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
```

### 2. Download and build

```sh
git clone https://github.com/parhansson/KMotionX.git
cd KMotionX
./configure
make
```

Use `./configure --help` to see the available build and directory options.
Build outputs are placed in the repository's `bin/` directory.

### 3. Install

```sh
make install
```

The default prefix is `$HOME/.local`; application installation does not require
`sudo`. Add `$HOME/.local/bin` to your `PATH` if you want to invoke installed
commands by name.

For a system-wide installation:

```sh
./configure --prefix=/usr/local
make
sudo make install
```

Machine-file seeding uses the HOME visible to `make install`. For a system-wide
installation, prepare each user's machine configuration separately rather than
assuming a privileged install will populate that user's home.

### 4. Configure device access on Linux

Install the KFLOP udev rule:

```sh
sudo cp KMotionX/usb/etc/udev/rules.d/10.kflop.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```

The supplied rule grants device access to the `plugdev` group. Ensure your
user belongs to that group, or adapt the rule to your distribution's access
policy. Reconnect the controller after changing the rules.

## Installation layout

Paths below are relative to the configured prefix:

| Path | Contents |
|------|----------|
| `bin/` | User-invoked commands, including `kmxWeb` |
| `lib/kmx/` | Shared libraries |
| `libexec/kmx/` | Private helpers: `KMotionServer` and `tcc67` |
| `share/kmx/DSP_KFLOP/` | Bundled KFLOP DSP resources and headers |
| `share/kmx/DSP_KOGNA/` | Bundled KOGNA DSP resources and headers |
| `include/kmx/` | Public headers |

Private helpers do not need to be on `PATH`. Runtime paths follow the configure
directory options; `--libdir` controls where KMotionX libraries are installed,
not where dependency libraries are discovered.

Package staging is supported:

```sh
make install DESTDIR=/absolute/staging
```

`DESTDIR` is not included in compiled runtime paths. Staging installs skip
user-machine seeding.

## Machine configuration

Writable machine files are separate from installed libraries and resources.
A normal `make install` seeds this default machine root:

```text
$HOME/.kmotionx/default-machine/
├── data/
│   ├── emc.var
│   ├── Default.tbl
│   └── Kinematics.txt
└── c-programs/
    └── BlinkKFLOP.c
```

Each template is copied only if its destination does not exist. Reinstalling
does not overwrite edited machine files.

The supplied `Kinematics.txt` selects `Kinematics3Rod`. This is not a universal
machine default: check the kinematics, variables and tool table against your
hardware before running controller programs.

To select a different machine root, create or edit `$HOME/.kmxrc`:

```ini
machineDataPath=/home/alex/machines/router
```

Replace the example with your own absolute path. The value names the machine
root, not its `data/` subdirectory. Do not use quotes, `$HOME` or `~` in the
value; they are not expanded. Replace an existing `machineDataPath` entry rather
than adding a duplicate.

Every selected root must contain readable `data/emc.var`, `data/Default.tbl`
and `data/Kinematics.txt`. Default relative C-program names resolve under
`c-programs/`. Without an override, the default root above is used.

Optional user-wide language preferences belong in
`$HOME/.kmotionx/data/LocalLanguage.txt`, outside the machine root.

### Migrating older configurations

Back up your existing files before changing the layout. Copy the machine files
you need from legacy `Data` and `C Programs` directories into the new root's
lowercase `data/` and `c-programs/` directories, reviewing existing destinations
before replacing anything.

DSP resources are loaded from the installed `share/kmx` directories, not from
per-user `DSP_KFLOP` or `DSP_KOGNA` directories. Legacy uppercase language paths
are not used.

## Driver options

The default USB driver is libftdi. To build with the proprietary ftd2xx driver:

```sh
./configure --ftd2xx --ftd2xx-libdir=/absolute/path/to/driver/lib
make
```

The explicit driver directory is needed when the library is outside the
compiler's normal search path. `PKG_CONFIG` or `--pkg-config` can select the
pkg-config executable for libftdi builds.

## Running examples

Examples are built into `bin/`. Review their source and machine settings before
running them: examples can communicate with the controller and execute programs.
Ensure the machine is ready and its physical safety controls are available.

For example, from the repository root:

```sh
./bin/executeGCode
```

`KMotionServer` starts automatically when controller communication requires it.
After updating the libraries, stop the existing server and restart client
applications so they use the matching versions. Do this only when the machine
is idle; do not interrupt a running operation.

## Updating and uninstalling

After updating the source, configure, rebuild and reinstall:

```sh
./configure
make
make install
```

Repeat any custom configure options used for your original installation.
Restart applications after updating.

To remove the installed KMotionX components:

```sh
make uninstall
```

Use the configuration corresponding to the installation you want to remove.
Uninstall preserves machine files and user-wide preferences.

## Further documentation

- [Troubleshooting](KMotionX/doc/Troubleshooting.md)
- [Advanced configuration](KMotionX/doc/Advanced.md)
- [Demonstration video](https://www.youtube.com/watch?v=oPTJwcre0hA)
