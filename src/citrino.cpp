/*
 * citrino.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */
#include "citrino.h"

SIGF_DECL(cback) {
	wireval_t wireval = GET_WIREVAL(modid, portid);
	uint8_t val = wireval_u8(wireval);
	printf("Signal. Module: %d Port: %d Wire: %d Val: %d -> ", modid, portid, wireid, val);
	print_wireval(wireval);
	printf("\n");
}

MODULE_CONSTR(Citrino) {
	PORT_CONNECT_IND(mem, out, out);
	ONCHANGE(out, cback);
}

MODULE_UPDATE(Citrino) {

}

MODULE_INITIAL(Citrino) {

}
