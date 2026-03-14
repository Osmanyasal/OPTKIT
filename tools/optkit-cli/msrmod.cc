#include <iostream>

#include "utils.hh"
#include "utils/utils.hh"

void execute_msrmod_command(const CommandArgs &args)
{
	if (args.msr_op == MsrOp::NONE)
	{
		std::cerr << "Error: msrmod requires -r or -w\n\n";
		print_help();
		return;
	}

	if (args.msr_op == MsrOp::READ)
	{
		uint64_t value = 0;
		if (!optkit::utils::read_msr(static_cast<int32_t>(args.msr_cpu), static_cast<off_t>(args.msr_address), &value, false))
		{
			std::cerr << "Error: failed to read MSR 0x" << std::hex << args.msr_address << std::dec
					  << " on cpu " << args.msr_cpu << " (is msr-safe loaded and permitted?)\n";
			return;
		}

		std::cout << std::hex << value << std::dec << "\n";
		return;
	}

	// WRITE
	if (!optkit::utils::write_msr(static_cast<int32_t>(args.msr_cpu), static_cast<off_t>(args.msr_address), args.msr_value, false))
	{
		std::cerr << "Error: failed to write MSR 0x" << std::hex << args.msr_address << std::dec
				  << " on cpu " << args.msr_cpu << " (is msr-safe loaded and permitted?)\n";
		return;
	}
}
