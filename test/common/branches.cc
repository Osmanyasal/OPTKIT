#include <stdio.h>
#include <stdlib.h>
#include "branches.hh"

/* This code has 1,500,000 total branches       		*/
/*                 500,000 not-taken conditional branches	*/
/*                 500,000 taken conditional branches		*/
/*                 500,000 unconditional branches		*/
int branches(void)
{

#if OPTKIT_ENV_CPU_ARCH_X86_64
	asm("\txor %%ecx,%%ecx\n"
		"\tmov $500000,%%ecx\n"
		"test_loop:\n"
		"\tjmp test_jmp\n"
		"\tnop\n"
		"test_jmp:\n"
		"\txor %%eax,%%eax\n"
		"\tjnz test_jmp2\n"
		"\tinc %%eax\n"
		"test_jmp2:\n"
		"\tdec %%ecx\n"
		"\tjnz test_loop\n"
		:					   /* no output registers */
		:					   /* no inputs */
		: "cc", "%ecx", "%eax" /* clobbered */
	);
	return 0;

#elif OPTKIT_ENV_CPU_ARCH_ARM32
	/* Initial code contributed by sam wang linux.swang _at_ gmail.com */

	asm("\teor r3,r3,r3\n"
		"\tldr r3,=500000\n"
		"test_loop:\n"
		"\tB test_jmp\n"
		"\tnop\n"
		"test_jmp:\n"
		"\teor r2,r2,r2\n"
		"\tcmp r2,#1\n"
		"\tbge test_jmp2\n"
		"\tnop\n"
		"\tadd r2,r2,#1\n"
		"test_jmp2:\n"
		"\tsub r3,r3,#1\n"
		"\tcmp r3,#1\n"
		"\tbgt test_loop\n"
		:				   /* no output registers */
		:				   /* no inputs		 */
		: "cc", "r2", "r3" /* clobbered */
	);

	return 0;

#elif OPTKIT_ENV_CPU_ARCH_ARM64
	asm("\teor x3,x3,x3\n"
		"\tldr x3,=500000\n"
		"test_loop:\n"
		"\tB test_jmp\n"
		"\tnop\n"
		"test_jmp:\n"
		"\teor x2,x2,x2\n"
		"\tcmp x2,#1\n"
		"\tbge test_jmp2\n"
		"\tnop\n"
		"\tadd x2,x2,#1\n"
		"test_jmp2:\n"
		"\tsub x3,x3,#1\n"
		"\tcmp x3,#1\n"
		"\tbgt test_loop\n"
		:				   /* no output registers */
		:				   /* no inputs		 */
		: "cc", "x2", "x3" /* clobbered */
	);

	return 0;

#elif OPTKIT_ENV_CPU_ARCH_RISCV32 || OPTKIT_ENV_CPU_ARCH_RISCV64
	asm volatile(
		"li t0, 500000\n" // t0 = loop counter
		"test_loop:\n"
		"j test_jmp\n" // unconditional branch (always taken)
		"nop\n"
		"test_jmp:\n"
		"xor t1, t1, t1\n"			// t1 = 0
		"bne t1, zero, test_jmp2\n" // branch not taken (t1 == 0)
		"addi t1, t1, 1\n"			// t1 = 1
		"test_jmp2:\n"
		"addi t0, t0, -1\n"	   // t0--
		"bnez t0, test_loop\n" // branch taken until t0 == 0
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
		"test_loop:\n"
		"\tb    test_jmp\n"
		"\tnop\n"
		"test_jmp:\n"
		"\txor  4,4,4\n"
		"\tcmpwi        cr0,4,1\n"
		"\tbge  test_jmp2\n"
		"\tnop\n"
		"\taddi 4,4,1\n"
		"test_jmp2:\n"
		"\taddi 3,3,-1\n"
		"\tcmpwi        cr0,3,1\n"
		"\tbgt  test_loop\n"
		:					/* no output registers */
		:					/* no inputs           */
		: "cr0", "r3", "r4" /* clobbered */
	);

	return 0;

#endif

	return -1;
}

int random_branches(int number, int quiet)
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
