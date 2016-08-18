/*
 * port.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef PORT_H_
#define PORT_H_

#include <vector>
#include <iostream>
#include "module.h"

enum PortDir {
	PORT_INPUT,
	PORT_OUTPUT,
	PORT_INOUT
};

enum PortType {
	PORT_REG,
	PORT_WIRE
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
	PORT_DRIVE_ERROR_OUTOFBOUNDS
};

enum AssignType {
	ASSIGN_CBACK, /* Assignment is resolved by a callback */
	ASSIGN_COND,  /* Assignment is resolved by a condition */
	ASSIGN_CONST  /* Assignment is resolved by a constant value */
};

/* Signal callback type: */
typedef void (*sig_raise_t)(uint32_t modid, uint32_t portid, uint32_t wireid, WireVal wire_logicval, WireEdge edge); /* Logic val can be: X, Z, 0 and 1 */

/* Signal struct type: */
typedef struct signal {
	sig_raise_t raise; /* Signal callback. Each wire can have a callback */
	WireEdge edge_trigger; /* What edge will this signal trigger? */
} signal_t;

typedef struct wire {
	WireVal val;                     /* X, Z, 0, 1 */
	WireEdge edge;                   /* Wire edge sensitivity. Can be posedge or negedge */
	WireEdge old_edge;               /* Store old edge in order to detect a change in logic */
	uint32_t modules_connected;      /* How many modules share this wire */
	uint32_t modules_connected_orig; /* Original value of the previous variable */
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
	PortDir dir;                     /* Input, Output, Inout */
	PortType type;                   /* Reg or Wire */
	bool synced;                     /* Synchronous or asynchronous */
	bool oe;                         /* Output Enable */
	std::vector<wire_t>   * wires;   /* Wires */
	std::vector<signal_t> * signals; /* Signal callbacks. Each wire can have a callback, but each port may have different signals */
	std::vector<assign_t> * assigns; /* Wire assignments */
	bool wires_dirty = 0;

public:
	Port(Module * ctx, PortDir dir, PortType type, uint32_t port_width, uint32_t default_val);

	void connect(Port* dst_port); /* Links two wires together */

	 /* Calls a callback, which then returns the value into a wire, in every cycle: */
	void assign(assign_ret_t assign_cback, uint32_t wire_off, uint32_t wire_len, void * cback_args);
	void assign(assign_ret_t assign_cback, uint32_t wire_off, uint32_t wire_len);
	void assign(assign_ret_t assign_cback, uint32_t nth_wire, void * cback_args);
	void assign(assign_ret_t assign_cback, uint32_t nth_wire);
	void assign(assign_ret_t assign_cback, void * cback_args);
	void assign(assign_ret_t assign_cback);

	/* Same as 'assign', but resolves a condition instead of a callback's return value: */
	void assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t wire_off, uint32_t wire_len, void * cback_args);
	void assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t wire_off, uint32_t wire_len);
	void assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t nth_wire, void * cback_args);
	void assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t nth_wire);
	void assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, void * cback_args);
	void assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side);

	/* Assign a constant wire value to a single wire, or a set of wires: */
	void assign_const(wireval_t const_val, uint32_t wire_off, uint32_t wire_len, void * cback_args);
	void assign_const(wireval_t const_val, uint32_t wire_off, uint32_t wire_len);
	void assign_const(wireval_t const_val, uint32_t nth_wire, void * cback_args);
	void assign_const(wireval_t const_val, uint32_t nth_wire);
	void assign_const(wireval_t const_val, void * cback_args);
	void assign_const(wireval_t const_val);

	void update_assign_condition(uint32_t nth_condition, uint8_t new_cond);

	PortDriveError drive(uint32_t wire_offset, uint32_t wire_length, std::vector<WireVal> wire_valdrive);
	PortDriveError drive_all(std::vector<WireVal> wire_valdrive);
	void set_sensitivity(uint32_t nth_wire, WireEdge edge, sig_raise_t sig_cback);
	void set_sensitivity_bus(sig_raise_t sig_cback);
	void update(uint32_t modid, uint32_t portid);
	void clean_wires();
	uint32_t get_wire_width();
	std::vector<wire_t> * get_wires();
};

#endif /* PORT_H_ */
