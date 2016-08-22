/*
 * memory.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "memory.h"

char val = 0;

SIGF_DECL(cback) {
	Module * mod = GET_MODULE(remote_modid);
	wireval_t wireval = GET_WIREVAL(this_modid, this_portid);
	uint8_t val = wireval_u8(wireval);
	printf("Signal cycle: %d. Module: %s %d Foreign: %d This: %d Wire: %d Val: %d -> ", (uint32_t)Refresher::refresh_cycle,
			mod->name.c_str(), remote_modid, this_portid, remote_portid, wireid, val);
	print_wireval(wireval);
	printf("\n");

	DRIVEI_ALL(GET_MODULE(this_modid)->ports[1], 0);
}

MODULE_CONSTR(MainMemory) {
	PORT_CONNECT(cpu, clk, clk);
	clk->set_sensitivity(0, LEVELEDGE, cback);
	in->set_sensitivity(0, ALLEDGES, cback);
}

MODULE_UPDATE(MainMemory) {

}

MODULE_INITIAL(MainMemory) {

}
