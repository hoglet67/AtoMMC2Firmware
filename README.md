# AtoMMC2 Firmware Repository

## Introduction

AtoMMC is an a SD card based file system for the Acorn Atom, created
by Charlie Robson (SirMorris) in 2010. It uses a PIC microcontroller
to interface an SD card to the Acorn Atom, supporting a standard
FAT16/FAT32 file system.

This repository contains a community maintained copy of the sources
and binaries for the PIC firmware for AtoMMC V2.

The firmware in this repository primarily supports the AtoMMC V2
boards, which was sold by Charlie from 2010 to about 2020. But is has
also been back-ported to the more recent versions of AtoMMC.

There is a second repsitory for the 4K AtoMMC ROM that is fitted with
the Acorn Atom itself:
* <https://github.com/hoglet67/AtoMMC2>

## History of AtoMMC

In 2008 Charlie Robson started work on an MMC card based file system
for the Acorn Atom. The initial version used an MMC card connected to
the Atom's 6522.

For more information on AtoMMC V1 see:
* <https://arduinonut.blogspot.com/search/label/acorn%20atom>

In 2010 Charlie released a V2 board that connected to the Atom's
internal PL8 connector and used a PIC Microcontroller to interface an
SD card to the Acorn Atom. This on-board controller runs a stack built
on FatFS, allowing the SD card to use a standard FAT filesystem. Over
the next 10 years, many AtoMMC V2 boards were sold to members of the
Acorn community.

For more information on AtoMMC V2 see:
* <https://www.acornatom.nl/sites/atomreview/sd-files/atommmc2.html>

In 2020 a V3 board was released that swaped the MISO/MOSI signals on
the PIC, correcting an oversight with the V2 design. With this change
it is possible to use the PIC's MSSP hardware to access the SD Card,
rather than big-banging as was done in V2.

Later in 2020, a revised V3 board was released that moved IRQ/NMI from
RC6/7 (pins 25/26) to RA4/3 (pins 5/4). The allows RC6/7 to be used as
a serial port for debugging.

In this document, the first V3 board will be referred to as V3MINUS
and the second V3 board as V3PLUS.

It was hoped that all the boards "in the wild" would be V3PLUS boards,
but a small number of V3MINUS boards seem to have been sold as well.

The V3 design (specifically the V3PLUS) was released under an open
source licence and the official PCB and 3.0 firmware is available in a
seperate repository:
* <https://github.com/charlierobson/atommc-v3>

Later still in 2020, Roland Leurs created a V4 board, that is
electrically identical to the V3PLUS:
* <https://site.acornatom.nl/hardware/storage/atommc-v4/>
* <https://stardot.org.uk/forums/viewtopic.php?t=20836>

It's recommended that users with the V3MINUS, V3PLUS and V4 boards
install the newer firmware from Charlie's atommc-v3 repository. By
using the PIC’s MSSP hardware for SD card access, this version
effectively doubles system performance.

However, mostly for testing purposes, the V2 firmware in this
repository also supports the newer hardware, though with slower
performance.

## Archived releases

This repositoru included an archive of the community firmware binary
builds in the releases/ directory.

### releases/picfirmware:

This directory contains archived copies of the main firmware.

Versions 29-2D are for the V2 hardware:
* 29 - built from Charlie's 2.9 sources
* 2A - various development builds (kees and hoglet)
* 2B - stable release: added support for random access files
* 2C - stable release: added support for mkdir, rmdir, rename
* 2D - stable release: fix for IRQ initialization race condition (#A000 ROM)

Version 2E is a back port for the V3MINUS hardware
* 2E - same as 2D, but with MISO and MOSI swapped on the PIC

Version 2F is a back port for the V3PLUS (and V4) hardware
* 2F - same as 2E, but with IRQ moved from PIC pin 25 to PIC pin 6

### releases/bootloader:

This directory contains archived copies of the boot loader
firmware. This boot loader should only be used on the V2 hardare. I
believe the 20, 21 and 29 boot loader are functionally the same, only
the version number changed to keep it in lock-step with the main
firmware.

### releases/bootupdater:

This directory contains archived copies of the boot loader
updater. This acts as temporary main firmware, allowing the boot
loader to be updated. This should only be used on the V2 hardware.

## License

This code is licensed under a Creative Commons CC BY-NC-SA 4.0 license.

Commercial use is not permitted.

See:
LICENSE.txt

See:
<https://creativecommons.org/licenses/by-nc-sa/4.0/>
