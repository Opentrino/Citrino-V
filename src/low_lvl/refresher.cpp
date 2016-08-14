/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "refresher.h"
#include "module.h"
#include "wireval.h"

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
				mod->ports[i]->update(mod->mod_id, i);
			mod->update(); /* Update the component itself (based on the updates on the ports (or not)) */
		}
	}
}

void Refresher::init_all() {
	for(Module * mod : modules) /* Run the 'initial' function for all components */
		mod->initial();
}

Module * Refresher::get_module(uint32_t modid) {
	if(modid >= Refresher::modules.size()) return 0;
	return Refresher::modules[modid];
}

Port * Refresher::get_port(uint32_t modid, uint32_t portid) {
	Module * mod = get_module(modid);
	if(!mod || portid >= mod->ports.size()) return 0;
	return mod->ports[portid];
}

std::vector<wire_t> * Refresher::get_wires(uint32_t modid, uint32_t portid) {
	Module * mod = get_module(modid);
	if(!mod || portid > mod->ports.size()) return 0;

	Port * port = get_port(modid, portid);
	if(!port) return 0;
	return port->get_wires();
}

wire_t * Refresher::get_wire(uint32_t modid, uint32_t portid, uint32_t wireid) {
	Module * mod = get_module(modid);
	if(!mod || portid > mod->ports.size()) return 0;

	Port * port = get_port(modid, portid);
	if(!port || wireid >= port->get_wire_width()) return 0;
	return &(*port->get_wires())[wireid];
}

std::vector<WireVal> Refresher::get_wireval(uint32_t modid, uint32_t portid, uint32_t wireid) {
	std::vector<WireVal> ret;
	std::vector<wire_t> * wires = get_wires(modid, portid);
	if(wires)
		ret = wires_to_wireval(wires);
	return ret;
}
