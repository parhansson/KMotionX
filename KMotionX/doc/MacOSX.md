### Mac OS X
This is a really nice platform. Almost everything is installed already. Given that you are a developer and have the latest XCode installed.

Install pkg-config and libftdi with one package manager. pkg-config lets
`./configure` find libftdi and libusb even when they are outside system paths.

With [Homebrew](https://brew.sh):
```
brew install pkg-config libftdi libusb
```

Or with [MacPorts](https://www.macports.org):
```
sudo port install pkgconfig libftdi1 libusb
```

If pkg-config is already installed, you can omit it from the install command.
Check discovery with `pkg-config --modversion libftdi1 libusb-1.0` before
running `./configure`. If you installed dependencies under a custom prefix,
set `PKG_CONFIG_PATH` to its `lib/pkgconfig` directory as shown in the main
[build instructions](../../README.md#install-kmotionx).
