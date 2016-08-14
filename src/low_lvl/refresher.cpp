/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "refresher.h"
#include "module.h"

std::vector<Module*> Refresher::modules;
uint32_t Refresher::module_id = 0;
Refresher refresher;

void Refresher::add_component(Module * comp) {
	modules.push_back(comp);
}

void Refresher::refresh_all() {
	while(refreshing) {
		for(Module * mod : modules) { /* Update all components */
			for(size_t i = 0; i < mod->ports.size(); i++) /* Update all Ports of every component */
				mod->ports[i]->update(mod->mod_id);
			mod->update(); /* Update the component itself (based on the updates on the ports (or not)) */
		}
	}
}
