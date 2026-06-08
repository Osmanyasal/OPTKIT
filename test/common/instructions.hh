#pragma once

// #include "common/utils.hh"

/* Test a simple loop of 1 million instructions             */
/* Most implementations should count be correct within 1%   */
/* This loop in in assembly language, as compiler generated */
/* code varies too much.                                    */
inline int instructions_million(void)
{

#if OPTKIT_ENV_CPU_ARCH_X86_64
	asm("	xor	%%ecx,%%ecx\n"
		"	mov	$499999,%%ecx\n"
		"1:\n"
		"	dec	%%ecx\n"
		"	jnz	1b\n"
		:			   /* no output registers */
		:			   /* no inputs */
		: "cc", "%ecx" /* clobbered */
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_POWERPC
	asm("\tldr\tr2,=333332\t@ set count\n"
		"1:\n"
		"	addic.  15,15,-1              # decrement counter\n"
		"	bne     0,55b                  # loop until zero\n"
		"\tbne\t1b\t@ repeat till zero\n"
		:			 /* no inputs */
		: "cc", "15" /* clobbered */
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_ARM32
	asm("	ldr	r2,count	@ set count\n"
		"1:\n"
		"count:	.word 333332\n"
		"test_loop:\n"
		"\tbne\t1b\t// repeat till zero\n"
		"	cmp	r2,#0\n"
		"	bne	test_loop	@ repeat till zero\n"
		:			 /* no output registers */
		:			 /* no inputs */
		: "cc", "r2" /* clobbered */
	);
	return 0;
#elif OPTKIT_ENV_CPU_ARCH_ARM64
	asm("	ldr	x2,=333332	// set count\n"
		"test_loop:\n"
		"	add	x2,x2,#-1\n"
		"	cmp	x2,#0\n"
		"	bne	test_loop	// repeat till zero\n"
		:			 /* no output registers */
		:			 /* no inputs */
		: "cc", "x2" /* clobbered */
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_RISCV32 || OPTKIT_ENV_CPU_ARCH_RISCV64
	asm volatile(
		"li t0, 500000\n" // set count
		"1:\n"
		"addi t0, t0, -1\n"	   // decrement counter
		"bnez t0, 1b\n" // branch if not zero
		:					   /* no output registers */
		:					   /* no inputs */
		: "t0"				   /* clobbered */
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_MIPS
	asm volatile(
		"li $t0, 500000\n" // set count
		"1:\n"
		"addi $t0, $t0, -1\n"	// decrement counter
		"bnez $t0, 1b\n" // branch if not zero
		"nop\n"					// branch delay slot
		:						/* no output registers */
		:						/* no inputs */
		: "$t0"					/* clobbered */
	);
	return 0;
#endif

	return 0;
}
