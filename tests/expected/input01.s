	.text
.LC0:
	.string	"%d\n"
printint:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	ret

	.comm	xian,8,8
	.comm	ning,8,8
	.text
	.globl	main
	.type	main,@function
main:
	pushq 	%rbp
	movq	%rsp,%rbp
	movq	$5,%r8
	movq	%r8, xian(%rip)
	movq	$2,%r8
	movq	$3,%r9
	addq	%r8,%r9
	movq	%r9, ning(%rip)
	movq	xian(%rip), %r8
	movq	ning(%rip), %r9
	movq	$1,%r10
	addq	%r9,%r10
	cmpq	%r10,%r8
	jle	L1
	movq	xian(%rip), %r8
	movq	ning(%rip), %r9
	addq	%r8,%r9
	movq	%r9, %rdi
	call	printint
	jmp	L2
L1:
	movq	xian(%rip), %r8
	movq	ning(%rip), %r9
	subq	%r9,%r8
	movq	%r8, %rdi
	call	printint
L2:
	movq	$0,%r8
	movq	%r8, ning(%rip)
	movq	$7,%r8
	movq	%r8, xian(%rip)
L3:
	movq	xian(%rip), %r8
	movq	ning(%rip), %r9
	cmpq	%r9,%r8
	jle	L4
	movq	ning(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	ning(%rip), %r8
	movq	$1,%r9
	addq	%r8,%r9
	movq	%r9, ning(%rip)
	jmp	L3
L4:
	movq	$7,%r8
	movq	%r8, xian(%rip)
	movq	$0,%r8
	movq	%r8, ning(%rip)
L5:
	movq	ning(%rip), %r8
	movq	$1,%r9
	addq	%r8,%r9
	movq	%r9, ning(%rip)
	movq	ning(%rip), %r8
	movq	%r8, %rdi
	call	printint
	movq	xian(%rip), %r8
	movq	ning(%rip), %r9
	cmpq	%r9,%r8
	jge	L5
	movl $0, %eax
	popq     %rbp
	ret
