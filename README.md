# ulibSD
It's a library for use SD cards in SPI mode with uControllers, entirely written
in C. This library can work with SD cards and also has the possibility to
emulate the behavior in a PC file (GNU/Linux) using the macro _M_IX86. It's for
debugging purposes. The data transfer is oriented to 512 byte size,
remember this.

## Public methods
ulibSD has four public methods:

* SD_Init: Initialization the SD card.
* SD_Read: Read a single block of data.
* SD_Write: Write a single block of data.
* SD_Status: Allows know status of SD card.

Those methods require a device descriptor.

## How is possible port the code to my platform?

This library uses a `spi_io.h` header. Here are defined the low-level methods 
associated with the hardware. Those methods are:

* `SPI_Init`: Initialize SPI hardware.
* `SPI_RW`: Read/Write a single byte. Returns the byte that arrived.
* `SPI_Release`: Flush of SPI buffer.
* `SPI_CS_Low`: Selecting function in SPI terms, associated with SPI module.
* `SPI_CS_High`: Deselecting function in SPI terms, associated with SPI module.
* `SPI_Freq_High`: Setting frequency of SPI's clock to maximun possible.
* `SPI_Freq_Low`: Setting frequency of SPI's clock equal or lower than 400kHz.
* `SPI_Timer_On`: Start a non-blocking timer in milliseconds.
* `SPI_Timer_Status`: Check the status of non-blocking timer.
* `SPI_Timer_Off`: Stop of non-blocking timer.

You need write the proper code for this methods. I leave a `spi_io.c.example` 
file for use as guideline. I hope this helps to you understand how is the logic
of portability. This example is for KL25Z board using my OpenKL25Z framework.

Also you need verify and adapt the integer types in the `integer.h` file.

## Example of use

```c
SD_DEV dev[1];          // Create device descriptor
uint8_t buffer[512];    // Example of your buffer data
void main(void)
{
  SDRESULTS res;
  // Part of your initialization code here
  if(SD_Init(dev)==SD_OK)
  {
    // You can read the sd card. For example you can read from the second
    // sector the set of bytes between [04..20]:
    // - Second sector is 1
    // - Offset is 4
    // - Bytes to count is 16 (20 minus 4)
    res = SD_Read(dev, (void*)buffer, 1, 4, 16);
    if(res==SD_OK)
    {
      // Maybe you wish change the data on this sector:
      res = SD_Write(dev, (void*)buffer, 1);
      if(res==SD_OK)
      {
        // Some action here
      }
    }
  }
}
```

### Important

## About HW

I wanna make a comment about the CS line (chip select). If you use a SPI module
in your hardware don't use the CS automatic capability. When you send a command
package to SD card (command and argument), we will need the CS line in
low-level all the time. If you use the CS automatic capability this logic will
be broken.
Also I strongly recommend uses a hardware timer associated with `SPI_Timer`
methods.

## About license

This library was strongly inspired by elm-chang [FatFs](http://elm-chan.org/fsw/ff/00index_e.html) code.
In [reddit](https://www.reddit.com/r/microcontrollers/comments/3e71s8/its_a_library_for_use_sd_cards_in_spi_mode_with/) they ask me about if the
library is inspired or derived from Chan work. Mister Chan hasn't a SD library
formally, the code in general appear as API or sometime as a specific code to
the project that he works. I have many lines similar to Mister Chan code of
their SD driver. The spirit of this library is cover the semantics of SD cards
specifically. Now works with Cortex-M0/+, in a near future I hope write about
how work with 8-bit MCU successfully. Against any ambiguous interpretation I
think the correct license will be keep the original license used by Mister Chan
in the code of FatFS (BSD-style).

### License

Unless indicated other license:

```c
/*----------------------------------------------------------------------------/
/  ulibSD - Library for SD cards semantics            (C)Nelson Lombardo, 2015
/-----------------------------------------------------------------------------/
/ ulibSD library is a free software that opened under license policy of
/ following conditions.
/
/ Copyright (C) 2015, ChaN, all right reserved.
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/

// Derived from Mister Chan works on FatFs code (http://elm-chan.org/fsw/ff/00index_e.html):

/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.11                 (C)ChaN, 2015
/-----------------------------------------------------------------------------/
/ FatFs module is a free software that opened under license policy of
/ following conditions.
/
/ Copyright (C) 2015, ChaN, all right reserved.
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/
```
