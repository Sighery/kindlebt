Limitations  {#limitations}
===========================

## Kindle generation

There are many Kindle devices, as well as generations of some of these product
lines. This Bluetooth library targets 11th generation and later devices:
Kindle 11th generation (**KT5**), Kindle Paperwhite 11th generation (**PW5**),
Kindle Scribe 1st generation (**KS** / **KS1**), etc.

> [!warning]
> Older devices than KT5 and PW5 **will not work**, as those seem to have been
> based in BlueZ, rather than Bluedroid which is the Bluetooth stack Amazon
> has settled on since the 11th gen release.

Newer devices will likely still work, but compatibility cannot be confirmed
without manual testing.

So far, I have tested compatibility in the following devices:

* Kindle Paperwhite Signature Edition 11th generation (**PW5SE**)
* Kindle Scribe 1st generation (**KS** / **KS1**)
* Kindle Colorsoft Signature Edition 1st generation (**CS** / **CSSE**)

## Kindle firmwares

Kindle devices use an ARM CPU, but there have been changes in the ABI used,
which requires recompiling programs for different targets. Kindle devices used
to use ARM SF (Soft Float). From firmware version 5.16.3, Amazon switched all
Kindles supporting this newer firmware version to ARM HF (Hard Float).

> [!warning]
> This library will not support, at least for now, Soft Float firmware
> versions. This is because the Bluetooth functionality in Kindles is under
> active development, and older firmwares lack the functionality to make most
> of BLE work. This could be worked around by manually patching `ace_bt` and
> `btmanagerd` with the needed extra support to make BLE work, but it'd be
> quite an undertaking.

Any firmware from 5.16.3 (taking into account the previously mentioned device
limitations) should work with this library. So far, I have tested
compatibility with the following firmware versions (Bluetooth capabilities
should be the same across all devices past 11th gen, but this assumption is
untested):

* 5.17.2 (**KS1**)
* 5.18.1.0.1 (**CSSE**)

## Implemented functionality

Only a small subset of the total Bluetooth standard is implemented. You can
see an up to date list of functionality in the
[public API page](@ref KINDLEBT_PUBLIC_API).

Additionally, there might be some Bluetooth functionality not implemented at
the firmware or hardware level. For instance, I've noticed issues connecting
Bluetooth 4.2 keyboards (HID). There is no easy way to tell what functionality
is missing without trying to use it.


<div class="section_buttons">

| Read Previous      |                         Read Next |
| :----------------- | --------------------------------: |
| [Home](index.html) | [Installation](installation.html) |

</div>
