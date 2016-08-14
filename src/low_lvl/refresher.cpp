/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "refresher.h"
#include <iostream>
#include "module.h"

std::vector<Module*> Refresher::modules;
Refresher refresher;

void Refresher::add_component(Module * comp) {
	modules.push_back(comp);
}

void Refresher::update() {
	while(refreshing) {
		for(Module * mod : modules) { /* Update all components */
			for(size_t i = 0; i < mod->ports.size(); i++) /* Update all Ports of every component */
				mod->ports[i]->update();
			mod->update(); /* Update the component itself (based on the updates on the ports (or not)) */
		}
	}
}
