	.file	"check_cache3.cpp"
	.text
.lcomm _ZStL8__ioinit,1,1
	.globl	_Z11time_accessPVh
	.def	_Z11time_accessPVh;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11time_accessPVh
_Z11time_accessPVh:
.LFB6717:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	leaq	32(%rsp), %rbp
	.seh_setframe	%rbp, 32
	.seh_endprologue
	movq	%rcx, 32(%rbp)
/APP
 # 12 "check_cache3.cpp" 1
	CPUID
	RDTSC
	
 # 0 "" 2
/NO_APP
	movq	%rax, -8(%rbp)
	movq	32(%rbp), %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movl	%eax, -20(%rbp)
/APP
 # 23 "check_cache3.cpp" 1
	RDTSCP
	
 # 0 "" 2
/NO_APP
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	subq	-8(%rbp), %rax
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
	.align 8
.LC0:
	.ascii "First access time in CPU cycles (Possible Cache Miss): \0"
	.align 8
.LC1:
	.ascii "Second access time in CPU cycles (Possible Cache Hit): \0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB6718:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$576, %rsp
	.seh_stackalloc	576
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	call	__main
	movq	__imp_GetCurrentThread(%rip), %rax
	call	*%rax
	movl	$1, %edx
	movq	%rax, %rcx
	movq	__imp_SetThreadAffinityMask(%rip), %rax
	call	*%rax
	leaq	-96(%rbp), %rax
	addq	$512, %rax
	movq	%rax, 432(%rbp)
	movq	432(%rbp), %rax
/APP
 # 39 "check_cache3.cpp" 1
	CLFLUSH (%rax)
	
 # 0 "" 2
/NO_APP
	movl	$0, 444(%rbp)
	jmp	.L4
.L5:
	movl	444(%rbp), %eax
	movl	%eax, %edx
	movl	444(%rbp), %eax
	cltq
	movb	%dl, -96(%rbp,%rax)
	addl	$1, 444(%rbp)
.L4:
	cmpl	$511, 444(%rbp)
	jle	.L5
	movq	432(%rbp), %rax
	movq	%rax, %rcx
	call	_Z11time_accessPVh
	movq	%rax, 424(%rbp)
	movq	432(%rbp), %rax
	movq	%rax, %rcx
	call	_Z11time_accessPVh
	movq	%rax, 416(%rbp)
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdx
	movq	.refptr._ZSt4cout(%rip), %rax
	movq	%rax, %rcx
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movq	%rax, %rcx
	movq	424(%rbp), %rax
	movq	%rax, %rdx
	call	_ZNSolsEy
	movq	%rax, %rcx
	movq	.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(%rip), %rax
	movq	%rax, %rdx
	call	_ZNSolsEPFRSoS_E
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdx
	movq	.refptr._ZSt4cout(%rip), %rax
	movq	%rax, %rcx
	call	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
	movq	%rax, %rcx
	movq	416(%rbp), %rax
	movq	%rax, %rdx
	call	_ZNSolsEy
	movq	%rax, %rcx
	movq	.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(%rip), %rax
	movq	%rax, %rdx
	call	_ZNSolsEPFRSoS_E
	movl	$0, %eax
	addq	$576, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__tcf_0;	.scl	3;	.type	32;	.endef
	.seh_proc	__tcf_0
__tcf_0:
.LFB7223:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	_ZStL8__ioinit(%rip), %rax
	movq	%rax, %rcx
	call	_ZNSt8ios_base4InitD1Ev
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	_Z41__static_initialization_and_destruction_0ii;	.scl	3;	.type	32;	.endef
	.seh_proc	_Z41__static_initialization_and_destruction_0ii
_Z41__static_initialization_and_destruction_0ii:
.LFB7222:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	%edx, 24(%rbp)
	cmpl	$1, 16(%rbp)
	jne	.L10
	cmpl	$65535, 24(%rbp)
	jne	.L10
	leaq	_ZStL8__ioinit(%rip), %rax
	movq	%rax, %rcx
	call	_ZNSt8ios_base4InitC1Ev
	leaq	__tcf_0(%rip), %rax
	movq	%rax, %rcx
	call	atexit
.L10:
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	_GLOBAL__sub_I__Z11time_accessPVh;	.scl	3;	.type	32;	.endef
	.seh_proc	_GLOBAL__sub_I__Z11time_accessPVh
_GLOBAL__sub_I__Z11time_accessPVh:
.LFB7224:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movl	$65535, %edx
	movl	$1, %ecx
	call	_Z41__static_initialization_and_destruction_0ii
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section	.ctors,"w"
	.align 8
	.quad	_GLOBAL__sub_I__Z11time_accessPVh
	.ident	"GCC: (GNU) 11.2.0"
	.def	_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc;	.scl	2;	.type	32;	.endef
	.def	_ZNSolsEy;	.scl	2;	.type	32;	.endef
	.def	_ZNSolsEPFRSoS_E;	.scl	2;	.type	32;	.endef
	.def	_ZNSt8ios_base4InitD1Ev;	.scl	2;	.type	32;	.endef
	.def	_ZNSt8ios_base4InitC1Ev;	.scl	2;	.type	32;	.endef
	.def	atexit;	.scl	2;	.type	32;	.endef
	.section	.rdata$.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_, "dr"
	.globl	.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	.linkonce	discard
.refptr._ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_:
	.quad	_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_
	.section	.rdata$.refptr._ZSt4cout, "dr"
	.globl	.refptr._ZSt4cout
	.linkonce	discard
.refptr._ZSt4cout:
	.quad	_ZSt4cout
