# Kindle Bluetooth for 11th generation and up

The Bluetooth functionality this project targets is available since the 11th
generation Kindles (KT5, PW5). As of now, this project is just targeting HF
(Hard Float) Kindle firmwares (version 5.16.3 and up). SF (Soft Float)
firmwares are technically possible to support but out of scope for now.

Since the 11th generation, Amazon seems to have switched to Bluedroid for
their Bluetooth stack (10th gen seems to have used BlueZ). On top of it, they
have their own closed-source Bluetooth stack abstraction `ace_bt`. They have
built their own closed-source server `btmanagerd`, which uses `ace_bt` and
other proprietary libraries.

There is basically no documentation for `ace_bt` and the different Bluetooth
components on Kindles. That's why this library, `kindlebt`, will be our
wrapper around it, providing a simpler and documented interface that can be
used from applications and from other languages.

## Documentation

For more on installation, usage, limitations, etc, head over to the
[online documentation][manual].


[manual]: https://sighery.github.io/kindlebt/


<div class="section_buttons">

|                            Read Next |
| -----------------------------------: |
| [Limitations](manual/limitations.md) |

</div>
