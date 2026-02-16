Building  {#building}
=====================

This project makes use of [koxtoolchain][], [kindle-sdk][] and [Meson][].

## Getting the SDK

The SDK is really just a combination of koxtoolchain, which provides a cross
toolchain for a given Kindle system, and a kindle-sdk install on top, which
provides the proprietary C libraries included in a given Kindle device +
firmware version.

There are basically two choices here, either use a prepackaged release, or
build koxtoolchain + kindle-sdk yourself.

### Using a prepackaged SDK

I build and prepackage a few SDK versions. This includes a packaged
koxtoolchain + kindle-sdk for a given device/firmware version. You can find
these in the [kindle-sdk's releases tab][kindle-sdk releases].

Here an example for using the `scribe1-5.17.3` SDK target:

```sh
# Download release (you can also download it from your browser)
wget https://github.com/Sighery/kindle-sdk/releases/latest/download/scribe1-5.17.3.tar.gz

# Unpack release. Any path is fine as long as you keep track of it
tar -xvf scribe1-5.17.3.tar.gz -C ~/
```

### Building koxtoolchain/kindle-sdk

If the kindle-sdk targets I'm building don't work for you, you can build your
own. For that, I recommend looking at the
[`kindle-sdk` README][kindle-sdk README].

`koxtoolchain` also provides prebuilt packages that you can just extract.
Unless for some reason you really need to build koxtoolchain, it is
recommended you get the latest from their releases tab and unpack.
koxtoolchain builds will often fail due to flaky network requests, and even
when successful, will take around 30 minutes to complete.

## Building KindleBT

Once you have the SDK set up, you'll need to set up Meson. The steps for your
system will depend, so refer to the [Meson installation][]. If using Nix, the
provided `shell.nix` will do the trick.

Once you have the system dependencies figured out, we can finally move into
building `kindlebt` with Meson:

```sh
# Create the build dir. Any name is fine as long as you keep track of it
mkdir .build_hf

# Set up the project. Remember where you extracted the SDK to.
# In previous examples it was extracted to just ~/
meson setup --cross-file ~/x-tools/arm-kindlehf-linux-gnueabihf/meson-crosscompile.txt ./build_hf

# Compile
meson compile -C .build_hf/

# The build targets will be found inside the build dir
ls .build_hf/libkindlebt.so
```


[koxtoolchain]: https://github.com/koreader/koxtoolchain
[kindle-sdk]: https://github.com/Sighery/kindle-sdk
[Meson]: https://mesonbuild.com
[kindle-sdk releases]: https://github.com/Sighery/kindle-sdk/releases
[kindle-sdk README]: https://github.com/Sighery/kindle-sdk/blob/main/README.md
[Meson installation]: https://mesonbuild.com/Getting-meson.html


<div class="section_buttons">

| Read Previous                     |                 Read Next |
| :-------------------------------- | ------------------------: |
| [Installation](installation.html) | [Darkroot](darkroot.html) |

</div>
