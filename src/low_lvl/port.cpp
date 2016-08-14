/*
 * port.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "port.h"

Port::Port(PortDir dir, PortType type, uint32_t port_width, uint32_t default_val) {
	oe = 0;
	synced = 0; /* Asynchronous by default */
	this->dir = dir;
	this->type = type;
	/* Initialize internal wire: */
	for(uint32_t i = 0; i < port_width; i++) {
		wire_t wire;
		wire.val = default_val & (1 << i) ? _1 : _0;
		wire.edge = NULLEDGE;
		wire.old_edge = wire.edge;
		wire.signal.raise = 0; /* Null callback */
		wire.signal.edge_trigger = NULLEDGE; /* No sensitivity present */
		wires.push_back(wire);
	}
}

void Port::update() { /* Update Port by checking sensitivity list */
	std::vector<signal_t> sigs_called;

	for(size_t i = 0; i < wires.size(); i++) {
		if(wires[i].edge != wires[i].old_edge) {
			wires[i].old_edge = wires[i].edge; /* Update old edge */
			/* Check if the wire was pulled high or low by the driver (some other module): */
			if(wires[i].signal.raise &&
					((wires[i].signal.edge_trigger == wires[i].edge ||
							(wires[i].signal.edge_trigger == LEVELEDGE &&
									(wires[i].edge == POSEDGE || wires[i].edge == NEGEDGE)))
				|| wires[i].signal.edge_trigger == ALLEDGES)) {
				/* Check if this signal was already called previously */
				bool sig_already_called = 0;
				for(auto sig : sigs_called) {
					if(sig.raise == wires[i].signal.raise) {
						sig_already_called = 1;
						break;
					}
				}
				if(!sig_already_called) {
					sigs_called.push_back(wires[i].signal);
					wires[i].signal.raise(wires[i].val, wires[i].edge); /* Call callback if a delta signal is detected */
				}
			}
		}
	}
}

void Port::set_sensitivity(uint32_t nth_wire, WireEdge edge, sig_raise_t sig_cback) {
	if(nth_wire < 0 || nth_wire >= wires.size()) return;

	wires[nth_wire].signal.edge_trigger = edge;
	wires[nth_wire].signal.raise = sig_cback;
}

/* Drive partial part of the wires and try to match the wire_valdrive value in this window: */
PortDriveError Port::drive(uint32_t wire_offset, uint32_t wire_length, std::vector<WireVal> wire_valdrive) {
	if(type != PORT_REG) return PORT_DRIVE_ERROR_NOTAREG;

	if(wire_offset < 0 || wire_offset >= wires.size()) return PORT_DRIVE_ERROR_NOTAREG;

	for(uint32_t i = wire_offset, j = 0; (i < wires.size()) && (i < wire_offset + wire_length) && (j < wire_valdrive.size()); i++) {
		WireVal logic = wire_valdrive[j++];
		wires[i].edge = logic == _1 ? POSEDGE : logic == _0 ? NEGEDGE : logic == _X ? NULLEDGE : NOEDGE;
		wires[i].val = logic;
	}
	return PORT_DRIVE_OK;
}

/* Drive all the wires */
PortDriveError Port::drive_all(std::vector<WireVal> wire_valdrive) {
	return drive(0, wires.size(), wire_valdrive);
}

/* Connect destination port with this current source port: */
void Port::connect(Port* dst_port) {

}
