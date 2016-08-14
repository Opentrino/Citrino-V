/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include <vector>

#include "module.h"

class Refresher { /* Updates the physics of the circuit */
public:
	static std::vector<Module*> modules;
	bool refreshing = 1;
	static uint32_t module_id;

	static void add_component(Module * comp);
	void refresh_all();
	void init_all();

	static Module * get_module(uint32_t modid);
	static Port * get_port(uint32_t modid, uint32_t portid);
	static std::vector<wire_t> * get_wires(uint32_t modid, uint32_t portid);
	static wire_t * get_wire(uint32_t modid, uint32_t portid, uint32_t wireid);
	static std::vector<WireVal> get_wireval(uint32_t modid, uint32_t portid, uint32_t wireid);

	static uint32_t get_new_module_id() {
		return module_id++;
	}
};

extern Refresher refresher;
