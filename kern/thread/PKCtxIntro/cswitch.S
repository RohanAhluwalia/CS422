/*
 * void cswitch(struct kctx *from, struct kctx *to);
 */
	.globl cswitch
cswitch:
	movl	4(%esp), %eax	/* %eax <- from */
	movl	8(%esp), %edx	/* %edx <- to */

	/* TODO: save the old kernel context */

	
	/* Save the five basic memory addresses => simple indexing from beginning of struct pointer. */
	movl %esp, 0(%eax)
	movl %edi, 4(%eax)
	movl %esi, 8(%eax)
	movl %ebx, 12(%eax)
	movl %ebp, 16(%eax)

	/* The sixth memory address; eip (instruction pointer) value is the return address of the caller 
	stored at the esp (stack pointer) value by calling convention.*/

	movl 0(%esp), %edi    /* Move old eip to edi */
	movl %edi, 20(%eax)   /* Move old eip (in edi) to sixth position. */


    /* *********** */

	movl (%edx), %esp    /* Change our stack frame to the new location */
	movl 20(%edx), %edi  /* Move new eip to edi. */
	movl %edi, (%esp)    /* Move new eip to return address space (esp base 0) */

	movl 4(%edx), %edi 
	movl 8(%edx), %esi
	movl 12(%edx), %ebx
	movl 16(%edx), %ebp

	xor	%eax, %eax
	ret