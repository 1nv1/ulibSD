/*
 *  File: spi_io.c.example
 *  Author: Nelson Lombardo
 *  Year: 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License at the end of file.
 */

#include "spi_io.h"

/******************************************************************************
 Module Public Functions - Low level SPI control functions
******************************************************************************/

void SPI_Init (void) {

    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
    /*
     *SPI0 Clock gate control. 1 clock enabled
     */
    SIM_SCGC4 |= SIM_SCGC4_SPI0_MASK;
    /*
     * Multiplexing pines
     */
    PORTD_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK & (~PORT_PCR_SRE_MASK); //CS
    GPIOD_PDDR |= 1 << 0; // Pin is configured as general-purpose output, for the GPIO function.

    PORTD_PCR1 = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK & (~PORT_PCR_SRE_MASK);
    PORTD_PCR2 = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK & (~PORT_PCR_SRE_MASK);
    PORTD_PCR3 = PORT_PCR_MUX(2) | PORT_PCR_PE_MASK  | PORT_PCR_PS_MASK;

    /*
     * Bit 7 SPIE   = 0 Disables receive and mode fault interrupts
     * Bit 6 SPE    = 1 Enables the SPI system
     * Bit 5 SPTIE  = 0 Disables SPI transmit interrupts
     * Bit 4 MSTR   = 1 Sets the SPI module as a master SPI device
     * Bit 3 CPOL   = 0 Configures SPI clock as active-high
     * Bit 2 CPHA   = 0 First edge on SPSCK at start of first data transfer cycle
     * Bit 1 SSOE   = 1 Determines SS pin function when mode fault enabled
     * Bit 0 LSBFE  = 0 SPI serial data transfers start with most significant bit
     */
    SPI0_C1 = 0x50;
    /*
     * Bit 7 PMIE       = 0 SPI hardware match interrupt disabled
     * Bit 6            = 0 Unimplemented
     * Bit 5 TXDMAE     = 0 DMA request disabled
     * Bit 4 MODFEN     = 1 In master mode, ~SS pin function is automatic ~SS output
     * Bit 3 BIDIROE    = 0 SPI data I/O pin acts as input
     * Bit 2 RXDMAE     = 0 DMA request disabled
     * Bit 1 SPISWAI    = 0 SPI clocks operate in wait mode
     * Bit 0 SPC0       = 0 uses separate pins for data input and output
     */
    SPI0_C2 = 0x00;

    /*
     * Bit 7    SPRF    = 0 Flag is set when receive data buffer is full
     * Bit 6    SPMF    = 0 Flag is set when SPIx_M = receive data buffer
     * Bit 5    SPTEF   = 0 Flag is set when transmit data buffer is empty
     * Bit 4    MODF    = 0 Mode fault flag for master mode
     * Bit 3:0          = 0 Reserved
     */
    SPI0_S = 0x00;
}

BYTE SPI_RW (BYTE d) {
    while(!(SPI0_S & SPI_S_SPTEF_MASK));
    SPI0_D = d;
    while(!(SPI0_S & SPI_S_SPRF_MASK));
    return((BYTE)(SPI0_D));
}

void SPI_Release (void) {
    WORD idx;
    for (idx=512; idx && (SPI_RW(0xFF)!=0xFF); idx--);
}

inline void SPI_CS_Low (void) {
    GPIOD_PDOR &= ~(1 << 0); //CS LOW
}

inline void SPI_CS_High (void){
    GPIOD_PDOR |= (1 << 0); //CS HIGH
}

inline void SPI_Freq_High (void) {
    SPI0_BR = 0x00; // 24MHz / 2 = 12MHz
}

inline void SPI_Freq_Low (void) {
    SPI0_BR = 0x43; // 24MHz / 80 = 300kHz
}

void SPI_Timer_On (WORD ms) {
    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;  // Make sure clock is enabled
    LPTMR0_CSR = 0;                     // Reset LPTMR settings
    LPTMR0_CMR = ms;                    // Set compare value (in ms)
    // Use 1kHz LPO with no prescaler
    LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;
    // Start the timer and wait for it to reach the compare value
    LPTMR0_CSR = LPTMR_CSR_TEN_MASK;
}

inline BOOL SPI_Timer_Status (void) {
    return (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK) ? TRUE : FALSE);
}

inline void SPI_Timer_Off (void) {
    LPTMR0_CSR = 0;                     // Turn off timer
}

#ifdef SPI_DEBUG_OSC
inline void SPI_Debug_Init(void)
{
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; // Port A enable
    PORTA_PCR12 = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK  | PORT_PCR_PS_MASK;
    GPIOA_PDDR |= (1 << 12); // Pin is configured as general-purpose output, for the GPIO function.
    GPIOA_PDOR &= ~(1 << 12); // Off
}
inline void SPI_Debug_Mark(void)
{
    GPIOA_PDOR |= (1 << 12); // On
    GPIOA_PDOR &= ~(1 << 12); // Off
}
#endif

/*
The MIT License (MIT)

Copyright (c) 2015 Nelson Lombardo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
