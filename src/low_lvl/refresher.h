/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include <vector>
#include "port.h"
#include "../libs/tinythread/tinythread.h"

class Refresher { /* Updates the physics of the circuit */
public:
	static std::vector<Module*> modules;
	static bool refreshing;
	static uint32_t module_id;

	static void add_module(Module * comp);
	void run(); /* Runs the initials and updates in different threads */

	static Module * get_module(uint32_t modid);
	static Port * get_port(uint32_t modid, uint32_t portid);
	static std::vector<wire_t> * get_wires(uint32_t modid, uint32_t portid);
	static wire_t * get_wire(uint32_t modid, uint32_t portid, uint32_t wireid);
	static std::vector<WireVal> get_wireval(uint32_t modid, uint32_t portid);

	static uint32_t get_new_module_id() {
		return module_id++;
	}
};

extern Refresher refresher;
