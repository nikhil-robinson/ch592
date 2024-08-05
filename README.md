# CH592

An open source development environment (tooling, headers, examples) for the [CH592](https://www.wch-ic.com/products/CH592.html). All the examples are taken from [openwch/ch592](https://github.com/openwch/ch592)


## Getting Started

### GCC

These examples uses the customised  [MRS_Toolchain](http://www.mounriver.com/download) gcc compiler which supports customized hardware stacking function,

```
 __attribute__((interrupt("WCH-Interrupt-fast"))).
```
This statement tells GCC to perform a hardware push operation.

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
