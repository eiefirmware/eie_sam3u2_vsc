.thumb
.syntax unified


/*
* The goal is to get to C code as fast as possible and do the more involved stuff there.
* All that's really needed is to ensure the stack pointer is set.
*/
.text

.align
.global _start
.thumb_func
_start:
    LDR R0, =vector_table;
    LDR SP, [R0];
    LDR R0, =c_startup;
    BX R0;
