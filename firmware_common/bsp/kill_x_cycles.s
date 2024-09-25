/******************************************************************************
* File: kill_x_cycles.s                                                       *
******************************************************************************/
.thumb
.syntax unified

.text
.align

@ -----------------------------------------------------------------------------
@  kill_x_cycles(unsigned long int x)
@  This function passes roughly x cycles, where x is any 32-bit integer.
@  Because of the entry and exit times, it will never quite be exact, but the
@  larger x is, the less impact the overhead will be.  
@  The processor is fully awake during this time and simply kills instructions
@  by running through a loop.  
@  To be compatible with C code, the function parameter must use r0 as, by 
@  convention, that's where the compiler would put it.
@  All timing is shown in [ ].  The function overhead is subtracted off the value passed
@  in, then the value is reduced by the number of cycles it takes for each run of the kill_x_loop
@  There will be a remainder error here, but it's at most 2 cycles.
@ 
@  Requires:
@ 	- R0 is an unsigned value holding the number of cycles to be passed
@ 
@  Promises:
@ 	- Roughly x cycles will be passed.  At 12 MHz clock, each cycle is 83.3ns.
@    Therefore, the maximum time that can be passed is about 357 seconds.
@  - r0 is destroyed during this code, thus the function is not re-entrant.

#define OVERHEAD 7

.global kill_x_cycles
.thumb_func
kill_x_cycles:                          @ [1] Function entry
    CMP         r0, OVERHEAD            @ [1] Check if x is at least OVERHEAD.
    BLT         kill_x_cycles_end       @ [1] If x is less than the overhead, just exit
    SUB         r0, r0, OVERHEAD        @ [1] Reduce the count by OVERHEAD 

kill_x_loop:                            @ [3 cycle loop]
    SUBS        r0, r0, 3               @ [1] Subtract the loop cycle cost from the counter
    BPL         kill_x_loop             @ [2] Check if positive or zero and repeat if so

kill_x_cycles_end:                      @
    BX          LR                      @ [1] Move the return address back to the PC
