/*
 * memory.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "module_api.h"

MODULE_DECL(MainMemory)
public:
	PORT_NEW(out, PORT_OUTPUT, PORT_WIRE, 8,  0);
	PORT_NEW(in,  PORT_INPUT , PORT_WIRE, 32, 0);
	PORT_NEW(clk, PORT_INPUT , PORT_WIRE, 1,  0);
};

#endif /* MEMORY_H_ */
