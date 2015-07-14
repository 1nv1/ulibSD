# ulibSD
It's a library for use SD cards in SPI mode with uControllers, entirely written
in C. This library can work with SD cards and also has the possibility to
emulate the behavior in a PC file (GNU/Linux) using the macro _M_IX86. It's for
debugging purposes. The data transfer is oriented to 512 byte size,
remember this ;)

This library was strongly inspired by elm-chang [FatFs](http://elm-chan.org/fsw/ff/00index_e.html) code.

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

### Important

I wanna make a comment about the CS line (chip select). If you use a SPI module in your hardware don't use the CS automatic capability. When you send a command package to SD card (command and argument), we will need the CS line in low-level all the time. If you use the CS automatic capability this logic will be broken.
Also I strongly recommend uses a hardware timer associated with `SPI_Timer` methods.

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
