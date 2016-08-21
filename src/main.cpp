#include <iostream>
#include <stdio.h>
#include "citrino.h"
#include "memory.h"

int main() {
	std::cout<<">> Citrino-V Microprocessor Simulator <<\n";
	MODULE_OBJDECL(Citrino, cpu);
	MODULE_OBJDECL(MainMemory, mem);

	/* Run processes/initial and always blocks: */
	refresher.run();
	return 0;
}
