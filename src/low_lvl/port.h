/*
 * port.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef PORT_H_
#define PORT_H_

#include <stdint.h>
#include <vector>
#include <iostream>
#include "module.h"

enum PortDir {
	PORT_INPUT,  /* Input */
	PORT_OUTPUT, /* Output */
	PORT_INOUT,  /* Input and Output */
	PORT_NODIR   /* Internal port with no direction */
};

enum PortType {
	PORT_REG, /* Register (buffered/latched/synchronous) type */
	PORT_WIRE /* Wire (asynchronous) type*/
};

enum WireEdge {
	NEGEDGE,   /* Negative Edge */
	POSEDGE,   /* Positive Edge */
	LEVELEDGE, /* Both Posedge and Negedge */
	NOEDGE,	   /* Same as going from any other edge to Z */
	ALLEDGES,  /* All edges, even invalid ones, will trigger */
	NULLEDGE   /* Invalid Edge */
};

enum WireVal {
	_0,
	_1,
	_X,
	_Z
};

enum PortDriveError {
	PORT_DRIVE_OK,
	PORT_DRIVE_ERROR_NOTAREG,
	PORT_DRIVE_ERROR_ISINPUT,
	PORT_DRIVE_ERROR_OUTOFBOUNDS
};

enum PortAssignError {
	PORT_ASSIGN_OK,
	PORT_ASSIGN_ISINPUT
};

enum PortConnectError {
	PORT_CONNECT_OK,
	PORT_CONNECT_DST_NOTAWIRE
};

enum PortSensError {
	PORT_SENS_OK,
	PORT_SENS_OUTOFBOUNDS
};

enum AssignType {
	ASSIGN_CBACK, /* Assignment is resolved by a callback */
	ASSIGN_COND,  /* Assignment is resolved by a condition */
	ASSIGN_CONST  /* Assignment is resolved by a constant value */
};

/* Signal callback type: */
typedef void (*sig_raise_t)(
	uint32_t this_modid,  uint32_t remote_modid,
	uint32_t this_portid, uint32_t remote_portid,
	uint32_t wireid, WireVal wire_logicval, WireEdge edge
); /* Logic val can be: X, Z, 0 and 1 */

/* Signal struct type: */
typedef struct signal {
	sig_raise_t raise; /* Signal callback. Each wire can have a callback */
	WireEdge edge_trigger; /* What edge will this signal trigger? */
} signal_t;

typedef struct wire {
	WireVal  val;                    /* X, Z, 0, 1 */
	WireEdge edge;                   /* Wire edge sensitivity. Can be posedge or negedge */
	WireEdge old_edge;               /* Store old edge in order to detect a change in logic */
	uint32_t modules_connected;      /* How many modules share this wire */
	uint32_t modules_connected_orig; /* Original value of the previous variable */
	Module * last_mod_drive;         /* Last module that has driven this wire */
	Port   * last_port_drive;        /* Last module's port that has driven this wire */
	char     refcount;               /* How many modules tried to drive this wire in a single clock cycle */
} wire_t;

typedef std::vector<WireVal> wireval_t;

/* Assign function callback type: */
typedef wireval_t (*assign_ret_t)(void*);

typedef struct assign {
	assign_ret_t assign_func; /* Callback of the assign function */
	void * cback_args;        /* Callback argument pointer */
	uint32_t wire_off;        /* Offset wire to which we will apply the assign */
	uint32_t wire_len;        /* How many wires will we apply this assign to */
	std::vector<wire_t> * left_side;  /* Left side that will go into the wire if the condition is true */
	std::vector<wire_t> * right_side; /* Right side that will go into the wire if the condition is false */
	wireval_t left_side_val;  /* Same as left_side, but a wireval */
	wireval_t right_side_val; /* Same as right_side, but a wireval */
	uint8_t assign_type;      /* Is this assignment resolved by a callback, conditional or constant? */
	uint8_t condition;        /* The condition that decides which side is assigned. If assign_type is a constant, then this condition won't have any effect */
} assign_t;

class Port {
private:
	bool synced;                     /* Synchronous or asynchronous */
	bool oe;                         /* Output Enable */
	std::vector<wire_t>   * wires;   /* Wires */
	std::vector<signal_t> * signals; /* Signal callbacks. Each wire can have a callback, but each port may have different signals */
	std::vector<assign_t> * assigns; /* Wire assignments */
	bool wires_dirty = 0;
	uint32_t portid;

public:
	Module * modparent; /* What module does this port belong to */
	std::string name;   /* Port name */
	PortDir dir;        /* Input, Output, Inout */
	PortType type;      /* Reg or Wire */

	Port(Module * ctx, uint32_t portid, std::string port_name, PortDir dir, PortType type, uint32_t port_width, uint32_t default_val);

	PortConnectError connect(Port* dst_port); /* Links two wires together */

	 /* Calls a callback, which then returns the value into a wire, in every cycle: */
	PortAssignError assign(assign_ret_t assign_cback, uint32_t wire_off, uint32_t wire_len, void * cback_args);
	PortAssignError assign(assign_ret_t assign_cback, uint32_t wire_off, uint32_t wire_len);
	PortAssignError assign(assign_ret_t assign_cback, uint32_t nth_wire, void * cback_args);
	PortAssignError assign(assign_ret_t assign_cback, uint32_t nth_wire);
	PortAssignError assign(assign_ret_t assign_cback, void * cback_args);
	PortAssignError assign(assign_ret_t assign_cback);

	/* Same as 'assign', but resolves a condition instead of a callback's return value: */
	PortAssignError assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t wire_off, uint32_t wire_len, void * cback_args);
	PortAssignError assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t wire_off, uint32_t wire_len);
	PortAssignError assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t nth_wire, void * cback_args);
	PortAssignError assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t nth_wire);
	PortAssignError assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, void * cback_args);
	PortAssignError assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side);

	/* Assign a constant wire value to a single wire, or a set of wires: */
	PortAssignError assign_const(wireval_t const_val, uint32_t wire_off, uint32_t wire_len, void * cback_args);
	PortAssignError assign_const(wireval_t const_val, uint32_t wire_off, uint32_t wire_len);
	PortAssignError assign_const(wireval_t const_val, uint32_t nth_wire, void * cback_args);
	PortAssignError assign_const(wireval_t const_val, uint32_t nth_wire);
	PortAssignError assign_const(wireval_t const_val, void * cback_args);
	PortAssignError assign_const(wireval_t const_val);

	void update_assign_condition(uint32_t nth_condition, uint8_t new_cond);

	PortDriveError drive(uint32_t wire_offset, uint32_t wire_length, std::vector<WireVal> wire_valdrive);
	PortDriveError drive_all(std::vector<WireVal> wire_valdrive);
	PortSensError set_sensitivity(uint32_t nth_wire, WireEdge edge, sig_raise_t sig_cback);
	void set_sensitivity_bus(sig_raise_t sig_cback);
	void onchange(sig_raise_t sig_cback);
	void update(uint32_t modid, uint32_t portid);
	void clean_wires();
	uint32_t get_wire_width();
	std::vector<wire_t> * get_wires();
};

#endif /* PORT_H_ */
