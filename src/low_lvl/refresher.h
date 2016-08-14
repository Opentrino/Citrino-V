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

	static uint32_t get_new_module_id() {
		return module_id++;
	}
};

extern Refresher refresher;
