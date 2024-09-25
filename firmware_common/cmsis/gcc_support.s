/* Implement some intrinsics temporarily, won't be needed when upgrading to new CMSIS */
/* Not efficient, but easy */

.thumb
.syntax unified

.text

.global __RBIT
.thumb_func
__RBIT:
    RBIT R0, R0
    BX LR
