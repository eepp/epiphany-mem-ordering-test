# Epiphany memory ordering test

This is a simple test demonstrating that load operations using data
previously written do not always use the updated values on the Adapteva
Epiphany E16G301 device.


## Building

On the Parallella system itself, just run

    make

You may specify a number of `nop` loops between the write access and
the subsequent read access by defining the `NOP_COUNT` environment
variable, e.g.:

    make NOP_COUNT=15


## Running

Start the host program:

    ./host

Load and start the Epiphany programs:

    e-loader --start epi.srec 0 0 4 4


## What happens?

Host side:

  1. A named shared memory region is allocated.
  2. Every second, the program verifies the structures of each
     Epiphany core. If a given structure has its error flag on,
     the expected and read values are printed.

Epiphany side:

  1. The program attaches to the named shared memory region created
     by the host side.
  2. The appropriate structure for this core is retrieved.
  3. In an infinite loop:
     1. A value is stored at a location in shared memory, different
        from the the current value at that location.
     2. A configurable number of `nop` loops execute.
     3. A value is read from the same location in shared memory.
     4. Both written (expected) and read values are compared. If they
        are different, the error flag for this core is set, and the
        loop breaks.
