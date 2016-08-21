/*
 * memory.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "memory.h"

SIGF_DECL(cback) {
	wireval_t wireval = GET_WIREVAL(modid, portid);
	uint8_t val = wireval_u8(wireval);
	printf("Signal. Module: %d Port: %d Wire: %d Val: %d -> ", modid, portid, wireid, val);
	print_wireval(wireval);
	printf("\n");
}

MODULE_CONSTR(MainMemory) {
	ONCHANGE(out, cback);
	PORT_CONNECT_IND(cpu, out, out);
}

MODULE_UPDATE(MainMemory) {

}

MODULE_INITIAL(MainMemory) {

}
