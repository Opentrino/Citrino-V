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
	NULLEDGE   /* Invalid Edge */
};

enum WireVal {
	_0,
	_1,
	_X,
	_Z
};

/* Signal callback type: */
typedef void (*sig_raise_t)(WireVal wire_logicval, WireEdge edge); /* Logic val can be: X, Z, 0 and 1 */

/* Signal struct type: */
typedef struct signal {
	sig_raise_t raise; /* Signal callback. Each wire can have a callback */
	WireEdge edge_trigger; /* What edge will this signal trigger? */
} signal_t;

typedef struct wire {
	WireVal val; /* X, Z, 0, 1 */
	WireEdge edge; /* Wire edge sensitivity. Can be posedge or negedge */
	WireEdge old_edge; /* Store old edge */
	signal_t signal; /* Signal callbacks. Each wire can have a callback */
} wire_t;

class Port {
private:
	PortDir dir;   /* Input, Output, Inout */
	PortType type; /* Reg or Wire */
	bool synced;   /* Synchronous or asynchronous */
	bool oe;       /* Output Enable */
	std::vector<wire_t> wires; /* Wires */

public:
	Port(PortDir dir, PortType type, uint32_t port_width, uint32_t default_val);

	void connect(Port* dst_port);
	void drive(uint32_t wire_offset, uint32_t wire_length, uint32_t wire_valdrive);
	void drive_all(uint32_t wire_valdrive);
	void set_sensitivity(uint32_t nth_wire, WireEdge edge, sig_raise_t sig_cback);
	void update();
};

#endif /* PORT_H_ */
