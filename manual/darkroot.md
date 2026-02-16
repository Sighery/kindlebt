Darkroot  {#darkroot}
=====================

There are some parts of the KindleBT codebase that will remain
undocumented. These parts mainly have to do with existing data structures
used by the underlying Bluetooth library.

When developing, or debugging `kindlebt`, you might need to reference some of
these undocumented parts. For this, you should go through the **Building
KindleBT** section of the previous [Building][] document. Running
`meson setup` is paramount to this.

Once you have done so, subprojects will be set up, and then you can find
relevant documentation in the `darkroot` subproject. For instance, using
`ripgrep` to search for a specific enumerator:

```sh
rg --color=always -C3 -i 'STATUS_OUT_OF_MEMORY' subprojects/darkroot
```

[Building]: building.md


<div class="section_buttons">

| Read Previous             |             Read Next |
| :------------------------ | --------------------: |
| [Building](building.html) | [Topics](topics.html) |

</div>
