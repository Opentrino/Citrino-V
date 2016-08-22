/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include <vector>
#include <stdint.h>
#include "port.h"
#include "../libs/tinythread/tinythread.h"

typedef struct conn_sched {
	std::string mod_src_name; /* Connect destination module to what. When we schedule we don't have the pointer to it, but we have the name */
	std::string port_src_name; /* Name of the port to which we want to connect to */
	char dir; /* 0: Connect source to dest. 1: Connect dest to source */
	Port * dst;
} conn_sched_t;

enum CONN_DIR {
	CONN_SRC_TO_DST, /* Connect source module to destination module */
	CONN_DST_TO_SRC  /* Connect destination module to source module */
};

class Refresher { /* Updates the physics of the circuit */
public:
	static std::vector<Module*> modules;
	static std::vector<conn_sched_t> connection_schedules;
	static bool refreshing;
	static uint32_t module_id;

	static void add_module(Module * comp);
	void run(); /* Runs the initials and updates in different threads */

	static Module * get_module(uint32_t modid);
	static Port * get_port(uint32_t modid, uint32_t portid);
	static std::vector<wire_t> * get_wires(uint32_t modid, uint32_t portid);
	static wire_t * get_wire(uint32_t modid, uint32_t portid, uint32_t wireid);
	static std::vector<WireVal> get_wireval(uint32_t modid, uint32_t portid);

	static void connect_schedule(std::string mod_src_name, std::string port_src_name, Port * dst, CONN_DIR connection_direction);

	static uint32_t get_new_module_id() {
		return module_id++;
	}
};

extern Refresher refresher;
