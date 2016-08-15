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
bool Refresher::refreshing = 1;
Refresher refresher;

void Refresher::add_component(Module * comp) {
	modules.push_back(comp);
}

std::vector<tthread::thread*> thlist;
std::vector<int> joinlist;

void init_dispatch_call(void * arg) {
	Module * mod = Refresher::get_module((uint32_t)arg);
	if(mod)
		mod->initial();
	joinlist.push_back((int)arg);
}

void refresh_always() { /* Arg will be ignored */
	while(Refresher::refreshing) {
		/* Join all threads: */
		for(int i = 0; i < (int)joinlist.size(); i++) {
			thlist[joinlist[i]]->join();
			delete thlist[joinlist[i]];
			joinlist.erase(joinlist.begin() + i);
		}
		for(Module * mod : Refresher::modules) { /* Update all components */
			for(size_t i = 0; i < mod->ports.size(); i++) /* Update all Ports of every component */
				mod->ports[i]->update(mod->mod_id, i);
			mod->update(); /* Update the component itself (based on the updates on the ports (or not)) */
		}
	}
}

void Refresher::run() {
	/* Run the 'initial' function for all modules in separate threads: */
	for(int i = 0; i < (int)modules.size(); i++)
		thlist.push_back(new tthread::thread(init_dispatch_call, (void*)(i)));
	/* Run the updates on the main thread: */
	refresh_always();
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
