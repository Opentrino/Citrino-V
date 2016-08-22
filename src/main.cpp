#include <iostream>
#include <stdio.h>
#include "citrino.h"
#include "memory.h"

int main() {
	std::cout<<">> Citrino-V Microprocessor Simulator <<\n";
	MODULE_OBJDECL(MainMemory, mem);
	MODULE_OBJDECL(Citrino,    cpu);

	/* Run processes/initial and always blocks: */
	refresher.run();

	std::cout<<"\n>> Finished <<\n";
	return 0;
}
