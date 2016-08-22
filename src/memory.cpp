/*
 * memory.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "memory.h"

SIGF_DECL(cback) {
	Module * mod = GET_MODULE(remote_modid);
	wireval_t wireval = GET_WIREVAL(this_modid, this_portid);
	uint8_t val = wireval_u8(wireval);
	printf("Signal. Module: %s %d Foreign: %d This: %d Wire: %d Val: %d -> ", mod->name.c_str(), remote_modid, this_portid, remote_portid, wireid, val);
	print_wireval(wireval);
	printf("\n");
}

MODULE_CONSTR(MainMemory) {
	PORT_CONNECT(cpu, clk, clk);
	clk->set_sensitivity(0, LEVELEDGE, cback);
}

MODULE_UPDATE(MainMemory) {

}

MODULE_INITIAL(MainMemory) {

}
