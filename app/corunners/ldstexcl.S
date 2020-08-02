/**
 * write_atomic
 *
 * arguments:
 * x0: value to store
 * x1: pointer to memory location where to store the value
 *
 * return:
 * w0: result of the store, 0 successful, 1 unsuccessful
 */
	.text

	.align 6

	.globl write_atomic
write_atomic:
	// mrs x2, daif              // get DAIF flags
	// msr daifset, #2           // disable IRQ
    // have to first load-acquire exclusive
    // otherwise the store-release exclusive
    // instruction won't work
    ldaxr x4, [x1]
store_excl_loop:
	stlxr w3, x0, [x1]
	cbnz w3, store_excl_loop
	dmb ish
	// msr daif, x2              // restore DAIF flags
    mov w0, w3
	ret
