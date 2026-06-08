#pragma once

#include "common/utils.hh"

/* This code has 1,500,000 total branches       		*/
/*                 500,000 not-taken conditional branches	*/
/*                 500,000 taken conditional branches		*/
/*                 500,000 unconditional branches		*/
inline int branches(void)
{

#if OPTKIT_ENV_CPU_ARCH_X86_64
	asm("\txor %%ecx,%%ecx\n"
		"\tmov $500000,%%ecx\n"
		"1:\n"
		"\tjmp 2f\n"
		"\tnop\n"
		"2:\n"
		"\txor %%eax,%%eax\n"
		"\tjnz 3f\n"
		"\tinc %%eax\n"
		"3:\n"
		"\tdec %%ecx\n"
		"\tjnz 1b\n"
		:					   /* no output registers */
		:					   /* no inputs */
		: "cc", "%ecx", "%eax" /* clobbered */
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_ARM32
	/* Initial code contributed by sam wang linux.swang _at_ gmail.com */

	asm("\teor r3,r3,r3\n"
		"\tldr r3,=500000\n"
		"1:\n"
		"\tB 2f\n"
		"\tnop\n"
		"2:\n"
		"\teor r2,r2,r2\n"
		"\tcmp r2,#1\n"
		"\tbge 3f\n"
		"\tnop\n"
		"\tadd r2,r2,#1\n"
		"3:\n"
		"\tsub r3,r3,#1\n"
		"\tcmp r3,#1\n"
		"\tbgt 1b\n"
		:				   /* no output registers */
		:				   /* no inputs		 */
		: "cc", "r2", "r3" /* clobbered */
	);

	return 0;

#elif OPTKIT_ENV_CPU_ARCH_ARM64
	asm("\teor x3,x3,x3\n"
		"\tldr x3,=500000\n"
		"1:\n"
		"\tB 2f\n"
		"\tnop\n"
		"2:\n"
		"\teor x2,x2,x2\n"
		"\tcmp x2,#1\n"
		"\tbge 3f\n"
		"\tnop\n"
		"\tadd x2,x2,#1\n"
		"3:\n"
		"\tsub x3,x3,#1\n"
		"\tcmp x3,#1\n"
		"\tbgt 1b\n"
		:				   /* no output registers */
		:				   /* no inputs		 */
		: "cc", "x2", "x3" /* clobbered */
	);

	return 0;

#elif OPTKIT_ENV_CPU_ARCH_RISCV32 || OPTKIT_ENV_CPU_ARCH_RISCV64
	asm volatile(
		"li t0, 500000\n" // t0 = loop counter
		"1:\n"
		"j 2f\n" // unconditional branch (always taken)
		"nop\n"
		"2:\n"
		"xor t1, t1, t1\n"			// t1 = 0
		"bne t1, zero, 3f\n" // branch not taken (t1 == 0)
		"addi t1, t1, 1\n"			// t1 = 1
		"3:\n"
		"addi t0, t0, -1\n"	   // t0--
		"bnez t0, 1b\n" // branch taken until t0 == 0
		:					   /* no outputs */
		:					   /* no inputs */
		: "t0", "t1"		   // clobbered: loop counter, temp register
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_POWERPC
	/* Not really optimized */

	asm("\txor  3,3,3\n"
		"\tlis  3,500000@ha\n"
		"\taddi 3,3,500000@l\n"
		"1:\n"
		"\tb    2f\n"
		"\tnop\n"
		"2:\n"
		"\txor  4,4,4\n"
		"\tcmpwi        cr0,4,1\n"
		"\tbge  3f\n"
		"\tnop\n"
		"\taddi 4,4,1\n"
		"3:\n"
		"\taddi 3,3,-1\n"
		"\tcmpwi        cr0,3,1\n"
		"\tbgt  1b\n"
		:					/* no output registers */
		:					/* no inputs           */
		: "cr0", "r3", "r4" /* clobbered */
	);

	return 0;

#endif

	return -1;
}

inline int random_branches(int number, int quiet)
{

	int j, junk = 0;
	double junk2 = 5.0;

	for (j = 0; j < number; j++)
	{

		if ((((random() >> 2) ^ (random() >> 4)) % 1000) > 500)
		{
			goto label_false;
		}

		/* can't just add, the optimizer is way too clever */
		junk++;
		junk2 *= junk;

		// printf("T");
	label_false:
		// printf("F");
		;
	}

	if (!quiet)
		printf("%lf\n", junk2);

	return junk;
}
