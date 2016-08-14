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

	/* Initialize internal wires and signals: */
	wires = new std::vector<wire_t>();
	signals = new std::vector<signal_t>();

	for(uint32_t i = 0; i < port_width; i++) {
		/* Setup a single wire: */
		wire_t wire;
		wire.val = default_val & (1 << i) ? _1 : _0;
		wire.edge = NULLEDGE;
		wire.old_edge = wire.edge;
		wires_dirty = 1;
		wire.modules_connected = 0;
		wire.modules_connected_orig = 0;
		wires->push_back(wire);

		/* Setup a single null signal for this particular wire on this particular port on this particular component: */
		signal_t nullsignal;
		nullsignal.raise = 0; /* Null callback */
		nullsignal.edge_trigger = NULLEDGE; /* No sensitivity present */
		signals->push_back(nullsignal);
	}
}

void Port::update(uint32_t modid) { /* Update Port by checking sensitivity list */
	std::vector<signal_t> sigs_called;

	for(size_t i = 0; i < (*wires).size(); i++) {
		if((*wires)[i].edge != (*wires)[i].old_edge) {
			/* Update old edge: */
			if(!(*wires)[i].modules_connected--) {
				/* Only update on the very last module that is using this wire: */
				(*wires)[i].old_edge = (*wires)[i].edge;
				(*wires)[i].modules_connected = (*wires)[i].modules_connected_orig; /* Restore module count value */
			}

			/* Check if the wire was pulled high or low by the driver (some other module): */
			if((*signals)[i].raise &&
					(((*signals)[i].edge_trigger == (*wires)[i].edge ||
							((*signals)[i].edge_trigger == LEVELEDGE &&
									((*wires)[i].edge == POSEDGE || (*wires)[i].edge == NEGEDGE)))
				|| (*signals)[i].edge_trigger == ALLEDGES))
			{
				/* Check if this signal was already called previously */
				bool sig_already_called = 0;
				for(auto sig : sigs_called) {
					if(sig.raise == (*signals)[i].raise) {
						sig_already_called = 1;
						break;
					}
				}
				if(!sig_already_called) {
					sigs_called.push_back((*signals)[i]);
					(*signals)[i].raise(modid, (*wires)[i].val, (*wires)[i].edge); /* Call callback if a delta signal is detected */
				}
			}
		}
	}
}

void Port::set_sensitivity(uint32_t nth_wire, WireEdge edge, sig_raise_t sig_cback) {
	if(nth_wire >= (*signals).size()) return;

	(*signals)[nth_wire].edge_trigger = edge;
	(*signals)[nth_wire].raise = sig_cback;
}

/* Drive partial part of the wires and try to match the wire_valdrive value in this window: */
PortDriveError Port::drive(uint32_t wire_offset, uint32_t wire_length, std::vector<WireVal> wire_valdrive) {
	if(type != PORT_REG) return PORT_DRIVE_ERROR_NOTAREG;
	if(wire_offset >= (*wires).size()) return PORT_DRIVE_ERROR_OUTOFBOUNDS;

	for(uint32_t i = wire_offset, j = 0; (i < (*wires).size()) && (i < wire_offset + wire_length) && (j < wire_valdrive.size()); i++) {
		WireVal logic = wire_valdrive[j++];
		(*wires)[i].edge = logic == _1 ? POSEDGE : logic == _0 ? NEGEDGE : logic == _X ? NULLEDGE : NOEDGE;
		(*wires)[i].val = logic;
	}
	return PORT_DRIVE_OK;
}

/* Drive all the wires */
PortDriveError Port::drive_all(std::vector<WireVal> wire_valdrive) {
	return drive(0, (*wires).size(), wire_valdrive);
}

/* Connect destination port with this current source port: */
void Port::connect(Port * dst_port) {
	if(dst_port->type == PORT_WIRE) {
		dst_port->clean_wires(); /* This is solely to prevent memory leak */
		dst_port->wires = this->wires; /* Connect wires */
		for(size_t i = 0; i < this->wires->size(); i++) {
			(*wires)[i].modules_connected++;
			(*wires)[i].modules_connected_orig = (*wires)[i].modules_connected;
		}
	}
}

void Port::clean_wires() {
	if(wires_dirty) {
		delete wires;
		wires_dirty = 0;
	}
}
