# CellPS3
code on PS3 Linux for Cell/B.E. sdk 3.1

# helloworld
Uses pthreads to run a hello world program on each SPU; code from here:

  https://web.cs.sunyit.edu/~blairw/CellSDK3.1Docs/programming/CBE_Programming_Tutorial_v3.1.pdf

# dma_tut*
Example of how to send data between the PPU/SPUs using the DMA in the form of an absolute value calculator.
Starts with a single SPE, then uses a single SPE with SPU SIMD intrinsics, then uses 4 SPEs. Code from here:

  https://www.kernel.org/pub/linux/kernel/people/geoff/cell/ps3-linux-docs/CellProgrammingTutorial/BasicsOfSPEProgramming.html

# euler*
Examples of how to start with a scalar/serial program and parallelize it with the Cell SDK, starts with euler and ends 
with euler_spe2. Code included with the Cell 3.1 SDK in the directory:

  /opt/cell/sdk/src
  
In the tutorial_source.tar file

# framebuffer_test
Drawing to the screen using the linux frame buffer; code from here:

  http://betteros.org/tut/graphics1.php#fbdev


