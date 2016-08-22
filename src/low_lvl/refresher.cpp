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
std::vector<conn_sched_t> Refresher::connection_schedules;
uint32_t Refresher::module_id = 0;
bool Refresher::refreshing = 1;
uint64_t Refresher::refresh_cycle = 0;
bool Refresher::driven_wires_on_update = 0;
Refresher refresher;

void Refresher::add_module(Module * comp) {
	modules.push_back(comp);
}

void Refresher::connect_schedule(std::string mod_src_name, std::string port_src_name, Port * dst, CONN_DIR connection_direction) {
	conn_sched_t conn_sched;
	conn_sched.dst = dst;
	conn_sched.dir = connection_direction;
	conn_sched.mod_src_name = mod_src_name;
	conn_sched.port_src_name = port_src_name;
	connection_schedules.push_back(conn_sched);
}

std::vector<tthread::thread*> thlist;
std::vector<int> joinlist;

void init_dispatch_call(void * arg) {
	Module * mod = Refresher::get_module((uint32_t)arg);
	if(mod)
		mod->initial();
	joinlist.push_back((int)arg);
}

/* Connect all the ports that are intended to be connected: */
void connect_all() {
	if(!Refresher::connection_schedules.size()) return; /* Nothing to connect */
	int i = 0;
	std::vector<int> erase_index;
	for(auto conn : Refresher::connection_schedules) {
		for(auto module : Refresher::modules)
			if(module->name == conn.mod_src_name)
				for(auto port : module->ports)
					if(port->name == conn.port_src_name) {
						/* We found the module and the port. We must connect it to the destination port: */
						if(conn.dir == CONN_SRC_TO_DST)
							port->connect(conn.dst);
						else if(conn.dir == CONN_DST_TO_SRC)
							conn.dst->connect(port);
						erase_index.push_back(i);
					}
		i++;
	}

	for(int idx : erase_index)
		Refresher::connection_schedules.erase(Refresher::connection_schedules.begin() + idx);
}

void refresh_always() {
	while(Refresher::refreshing) {
		/* Connect all scheduled wires (that were scheduled 'live'): */
		connect_all();

		/* Join all threads: */
		for(int i = 0; i < (int)joinlist.size(); i++) {
			thlist[joinlist[i]]->join();
			delete thlist[joinlist[i]];
			joinlist.erase(joinlist.begin() + i);
		}

		/* Update all modules: */
		char sub_refresh_count = 0;
		do {
			Refresher::driven_wires_on_update = 0;

			for(Module * mod : Refresher::modules) {
					/* Update all Ports of every module: */
					for(size_t i = 0; i < mod->ports.size(); i++)
						mod->ports[i]->update(mod->mod_id, (uint32_t)i);

					/* Update the module itself (based on the updates on the ports (or not)): */
					if(!sub_refresh_count) /* Only update once though */
						mod->update();
			}

			if(Refresher::driven_wires_on_update)
				sub_refresh_count++;
		} while(Refresher::driven_wires_on_update);

		Refresher::refresh_cycle++;
	}
}

void Refresher::run() {
	/* Connect all scheduled wires before everything starts running: */
	connect_all();
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

std::vector<WireVal> Refresher::get_wireval(uint32_t modid, uint32_t portid) {
	std::vector<WireVal> ret;
	std::vector<wire_t> * wires = get_wires(modid, portid);
	if(wires)
		ret = wires_to_wireval(wires);
	return ret;
}

static std::vector<wire_t> static_wire;

std::vector<wire_t> * wireval_to_wires(wireval_t wireval) {
	static_wire.clear();
	for(auto wire : wireval) {
		wire_t new_wire;
		new_wire.edge = NULLEDGE;
		new_wire.modules_connected = 0;
		new_wire.modules_connected_orig = 0;
		new_wire.old_edge = NULLEDGE;
		new_wire.val = wire; /* We only care about the value */
		static_wire.push_back(new_wire);
	}
	return &static_wire;
}
