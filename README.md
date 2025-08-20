# Kindle Bluetooth for PW5 and up

An attempt at trying to expose Bluetooth on Kindles for more useful
functionality.

The Bluetooth functionality this project targets is available since the Kindle
Paperwhite 11th generation (PW5). As far as I can tell, this includes both
Classic and Low Energy.

The Kindles have some limited UI to allow for very basic Classic connection and
audio playback.

This project links against the existing `ace_bt` library available on Kindles.
This library contains all the Bluetooth functionality, but it's a closed source
library with no documentation.

`kindlebt` will be our wrapper around it, providing a simpler interface that can
be used from other languages (such as Golang).

## How to build

The process is quite involved. I'm working on making Docker images available,
but it's still unfinished. For now, it requires setting up a manual cross
toolchain.

### Koxtoolchain

You'll need to install [koxtoolchain][]. This process will likely take a while.
Make sure you have `makeinfo >= 7.1.1` (`makeinfo --version`, comes from
`textinfo` package in Debian/Ubuntu). You can also use the prebuilt releases.
More information on the
[Kindle Modding Wiki][Kindle Modding Wiki prerequisites].

You'll need either `kindlepw2` or `kindlehf` targets.

### Kindle-SDK

Required to set up Kindle libraries like `ace_bt` and others inside the `kox`
toolchains. The [main release][kindle-sdk] is available but as of now, unusable
for this project:

* The only SF (Soft Float) target is `kindlepw2`, which doesn't have `ace_bt`
  libraries available, as it's a Kindle Paperwhite 6th gen (PW2).
* The only HF (Hard Float) target is `kindlehf`, which is using firmware from a
  PW4, which again doesn't have `ace_bt` libraries. This was somewhat fixed by
  them merging Scribe1 firmware on top which provides `ace_bt`, but leads to the
  next issue.
* The `kindlehf` is now using 5.16.3 Scribe1 `ace` libraries, but I couldn't get
  this project's headers to link against 5.16.3 and had to switch to 5.17.2
  instead.

Therefore, for now I recommend using [my fork][kindle-sdk fork]. You may also
follow the [Kindle Modding Wiki][Kindle Modding Wiki prerequisites] steps, just
switching the project URL. This version provides an expert mode to give custom
combinations of `toolchain`/`platform`/`firmware_url`. You can find more info on
its README.

### Meson

Once you have `koxtoolchain` and `kindle-sdk` for your target set up, it's time
to set up Meson in this project:

```sh
# Make build dir
mkdir .build

# Set up Meson. Pass your proper path in --cross-file
meson setup --wipe --cross-file ~/x-tools/arm-kindlehf-linux-gnueabihf/meson-crosscompile.txt .build/

# Compile with Meson
meson compile -C .build/
```

You'll find `libkindlebt.so` and `kindlebt_test` under the build dir (in this
example `.build/`).

## Useful Kindle Bluetooth information

The Kindles come with `ace_bt_cli`, a CLI utility wrapping the `ace_bt` library.
This tool is a bit particular, but I've compiled a
[recipe on a basic BLE interaction][ace_bt_cli recipe]. A list of the options
with some extra info [is also available][ace_bt_cli help].

Bluetooth operations seem to require using the `bluetooth` user and group. The
`ace_bt_cli` automatically drops privileges.

`ace` and `ace_bt` logs can be found under `/var/log/messages`. This file has
pretty aggressive rotation. Use something like
`tail -F /var/log/message | grep -i ace`.

HCI logging can be enabled and found under `/mnt/us/bt_snoop_log`. This is a
binary file format that can be inspected with Wireshark.

Sometimes the Bluetooth manager is finicky. You can restart it with
`initctl restart btmanagerd`. Give it some seconds to fully initialise.


[koxtoolchain]: https://github.com/koreader/koxtoolchain
[Kindle Modding Wiki prerequisites]: https://kindlemodding.org/kindle-dev/gtk-tutorial/prerequisites.html
[kindle-sdk]: https://github.com/KindleModding/kindle-sdk
[kindle-sdk fork]: https://github.com/Sighery/kindle-sdk
[ace_bt_cli recipe]: https://github.com/Sighery/kindle-notes/blob/master/bluetooth/kindle_pico_led_recipe.md
[ace_bt_cli help]: https://github.com/Sighery/kindle-notes/blob/master/bluetooth/ace_bt_cli_help.txt
