/*
 * void cswitch(struct kctx *from, struct kctx *to);
 */
	.globl cswitch
cswitch:
	movl	4(%esp), %eax	/* %eax <- from */
	movl	8(%esp), %edx	/* %edx <- to */

	/* TODO: save the old kernel context */

	
	/* Save the five basic memory addresses => simple indexing from beginning of struct pointer. */
	movl %esp, (%eax)
	movl %edi, 4(%eax)
	movl %esi, 8(%eax)
	movl %ebx, 12(%eax)
	movl %ebp, 16(%eax)

	/* The sixth memory address; eip (instruction pointer) value is the return address of the caller 
	stored at the esp (stack pointer) value by calling convention.*/
	movl (%eax), %edi 
	movl %edi, 20(%eax) 



	/* TODO: load the new kernel context */
	movl (%edx), %esp 
	movl 4(%edx), %edi 
	movl 8(%edx), %esi
	movl 12(%edx), %ebx
	movl 16(%edx), %ebp

	/** The sixth memory address (eip) should be stored at byte 20 in the kctx struct. We cannot directly manipulate it, so we set it as the return address of the caller by convention. for */
	movl 20(%edx), %esp



	xor	%eax, %eax
	ret
