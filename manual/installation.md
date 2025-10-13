Installation  {#installation}
=============================

Head on over to the [Releases tab][releases] and download the relevant file
for your device and firmware version.

> [!note]
> Right now, only a single HF target is provided, which should be universal
> for any/all devices and HF firmwares, but this might change in the future.

Once you have downloaded the release, you can unpack it and use
`libkindlebt.so` as any other C library:

```sh
# Unpack .tar.gz
tar -xvf kindlebt-hf.tar.gz

# Either copy to /usr/lib/ or use LD_LIBRARY_PATH
cp libkindlebt.so /usr/lib/.
./application_that_uses_kindlebt

# LD_LIBRARY_PATH if you don't want to touch the system
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./application_that_uses_kindlebt
```


[releases]: https://github.com/Sighery/kindlebt/releases


<div class="section_buttons">

| Read Previous                   |                 Read Next |
| :------------------------------ | ------------------------: |
| [Limitations](limitations.html) | [Building](building.html) |

</div>
