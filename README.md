# CH592

An open source development environment (tooling, headers, examples) for the [CH592](https://www.wch-ic.com/products/CH592.html) Based on [openwch/ch592](https://github.com/openwch/ch592).


## Getting Started

### GCC

These examples uses [XPACK](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack) gcc compiler. For more details please read the [Install](https://xpack.github.io/dev-tools/riscv-none-elf-gcc/install/) page.

### Build

```
cd examples/blink
make
make flash
```

## FLASHING

The elf can be flashed to the CH592 using [wchisp](https://github.com/ch32-rs/wchisp). (Enter the CH592 bootloader by holding down BOOT when connecting it using USB).\
The SDK for CH592 from the openwch EVT is vendored under ``sdk/``. (Encoding has been changed from gbk to utf-8). 

### Footnotes/links

https://www.wch-ic.com/products/CH592.html
https://github.com/openwch/ch592
https://github.com/Suniasuta/CH592_Makefile_Template
https://github.com/rgoulter/ch592-ble-hid-keyboard-example
https://github.com/cnlohr/ch32v003fun
