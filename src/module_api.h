/*
 * module_api.h
 *
 *  Created on: 21/08/2016
 *      Author: Miguel
 */

#ifndef MODULE_API_H_
#define MODULE_API_H_

#include <stdint.h>

#include "low_lvl/module.h"
#include "low_lvl/port.h"
#include "low_lvl/wireval.h"
#include "low_lvl/refresher.h"

#define STR(str) #str
#define STRING(str) STR(str)

/* Module API: */
#define PORT_INIT(PORT_PTR, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL) PORT_PTR = new Port(this, __COUNTER__, STRING(PORT_PTR), PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL)
#define PORT_DECL(PORT_NAME) Port * PORT_NAME
#define PORT_NEW(PORT_NAME, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL) Port * PORT_INIT(PORT_NAME, PORT_DIR, PORT_TYPE, PORT_WIDTH, DEFAULT_VAL)
#define PORT_CONNECT_DIRECT(PORT_SRC, PORT_DST) PORT_SRC->connect(PORT_DST)
#define PORT_CONNECT(MODULE_NAME, PORT_NAME, PORT_DST) Refresher::connect_schedule(STRING(MODULE_NAME), STRING(PORT_NAME), PORT_DST, PORT_DST->dir == PORT_OUTPUT ? CONN_DST_TO_SRC : CONN_SRC_TO_DST)

#define GET_WIREVAL(modid, portid) Refresher::get_wireval(modid, portid)
#define GET_MODULE(modid) Refresher::get_module(modid)
#define GET_PORT(modid, portid) Refresher::get_port(modid, portid)

#define ONCHANGE(port, cback) out->onchange(cback)

#define SIGF_DECL(FUNC_NAME) void FUNC_NAME(uint32_t this_modid, uint32_t remote_modid, uint32_t this_portid, uint32_t remote_portid, uint32_t wireid, WireVal wire_logicval, WireEdge edge)

/* Module Declaration API: */
#define MODULE_DECL(Classname) class Classname : public Module { public: Classname(); void update(); void initial(); private:
#define MODULE_OBJDECL(Classname, Objectname) Classname Objectname; Objectname.name = STRING(Objectname)
#define MODULE_CONSTR(Classname) Classname::Classname() : Module()
#define MODULE_UPDATE(Classname) void Classname::update()
#define MODULE_INITIAL(Classname) void Classname::initial()

#define DRIVE(port, wire_offset, wire_length, ...) port->drive(wire_offset, wire_length, {__VA_ARGS__})
#define DRIVE_ALL(port, ...) port->drive_all({__VA_ARGS__})
#define DRIVEI(port, wire_offset, wire_length, val) port->drive(wire_offset, wire_length, to_wireval(val))
#define DRIVEI_ALL(port, val) port->drive_all(to_wireval(val))

#endif /* MODULE_API_H_ */
