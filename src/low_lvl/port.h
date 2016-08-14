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

/* Signal callback type: */
typedef void (*sig_raise_t)(uint32_t modid, WireVal wire_logicval, WireEdge edge); /* Logic val can be: X, Z, 0 and 1 */

/* Signal struct type: */
typedef struct signal {
	sig_raise_t raise; /* Signal callback. Each wire can have a callback */
	WireEdge edge_trigger; /* What edge will this signal trigger? */
} signal_t;

typedef struct wire {
	WireVal val;       			     /* X, Z, 0, 1 */
	WireEdge edge;     			     /* Wire edge sensitivity. Can be posedge or negedge */
	WireEdge old_edge; 			     /* Store old edge in order to detect a change in logic */
	uint32_t modules_connected;      /* How many modules share this wire */
	uint32_t modules_connected_orig; /* Original value of the previous variable */
} wire_t;

class Port {
private:
	PortDir dir;                     /* Input, Output, Inout */
	PortType type;                   /* Reg or Wire */
	bool synced;                     /* Synchronous or asynchronous */
	bool oe;                         /* Output Enable */
	std::vector<wire_t> * wires;     /* Wires */
	std::vector<signal_t> * signals; /* Signal callbacks. Each wire can have a callback, but each port may have different signals */
	bool wires_dirty = 0;

public:
	Port(PortDir dir, PortType type, uint32_t port_width, uint32_t default_val);

	void connect(Port* dst_port);
	PortDriveError drive(uint32_t wire_offset, uint32_t wire_length, std::vector<WireVal> wire_valdrive);
	PortDriveError drive_all(std::vector<WireVal> wire_valdrive);
	void set_sensitivity(uint32_t nth_wire, WireEdge edge, sig_raise_t sig_cback);
	void update(uint32_t modid);
	void clean_wires();
};

#endif /* PORT_H_ */
