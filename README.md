Audio visualizer
===================

This is my BSC Thesis Project at Budapest University of Technology. The project is a home audio entertainment system, which plays your music or any audio and making mood lighting to it.

It’s a C program runs on a microcontroller, which responsible for realtime Analog-Digital Conversation, signal analisis and coloring leds.

----------


Hardwares:
-------------

> - Nucleo STM32 F446RE 
> - WS2812 


Software:
-------------

The board has its own ADC, the program using this for the conversation, with DMA.  The analisis is done with Fast Fourier Transformation and the data sending to the leds is done with SPI.
