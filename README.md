# ulibSD
It's a library for use SD cards in SPI mode with uControllers. This library can
work with SD cards and also has the possibility to emulate the behavior in a
PC file (GNU/Linux) using the macro _M_IX86. It's for debugging purposes.
ulibSD is oriented to 512 byte data block. Remember this ;)

## Public methods
ulibSD has four public methods:

* SD_Init: Initialization the SD card.
* SD_Read: Read a single block of data.
* SD_Write: Write a single block of data.
* SD_Status: Allows know status of SD card.

Those methods require a device descriptor.

## Example of use

```c
SD_DEV dev[1]; // Create device descriptor
uint8_t buffer; // Example of your buffer data
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
