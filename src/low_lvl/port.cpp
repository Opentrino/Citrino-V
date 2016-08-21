/*
 * port.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include <algorithm>
#include "port.h"
#include "wireval.h"

Port::Port(Module * ctx, std::string port_name, PortDir dir, PortType type, uint32_t port_width, uint32_t default_val) {
	oe = 0;
	synced = 0; /* Asynchronous by default */
	this->dir = dir;
	this->type = type;

	name = port_name;

	/* Initialize internal wires and signals: */
	wires = new std::vector<wire_t>();
	signals = new std::vector<signal_t>();
	assigns = new std::vector<assign_t>();

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

		/* Setup a single null signal for this particular wire on this particular port on this particular module: */
		signal_t nullsignal;
		nullsignal.raise = 0; /* Null callback */
		nullsignal.edge_trigger = NULLEDGE; /* No sensitivity present */
		signals->push_back(nullsignal);
	}
	ctx->addport(this);
}

void Port::update(uint32_t modid, uint32_t portid) { /* Update Port by checking sensitivity list */
	std::vector<signal_t> sigs_called;

	/* Update the assignments for this port: */
	for(int i = 0; i < (int)assigns->size(); i++) {
		assign_t assign = (*assigns)[i];
		switch(assign.assign_type) {
		case ASSIGN_CBACK: {
				/* Run callback and assign its return value to the selected wires: */
				if(assign.assign_func) {
					wireval_t wireval = assign.assign_func(assign.cback_args);
					std::vector<wire_t> * cback_ret = wireval_to_wires(wireval);
					for(int j = (int)(assign.wire_off), k = 0; j < (int)(assign.wire_off + assign.wire_len) && j < (int)(wires->size()) && k < (int)(cback_ret->size()); j++, k++) {
						WireVal logic = wireval[j];
						(*wires)[j].edge = logic == _1 ? POSEDGE : logic == _0 ? NEGEDGE : logic == _Z ? NOEDGE : NULLEDGE;
						(*wires)[j].val = (*cback_ret)[k].val;
					}
				}
			}
			break;
		case ASSIGN_COND: {
				/* Assign wires based on a condition. Remember that the condition itself must be updated as well */
				wireval_t wireval = assign.condition ? assign.left_side_val : assign.right_side_val;
				std::vector<wire_t> * assign_val = assign.condition ? assign.left_side : assign.right_side;
				for(int j = (int)(assign.wire_off), k = 0; j < (int)(assign.wire_off + assign.wire_len) && j < (int)(wires->size()) && k < (int)(assign_val->size()); j++, k++) {
					WireVal logic = wireval[j];
					(*wires)[j].edge = logic == _1 ? POSEDGE : logic == _0 ? NEGEDGE : logic == _Z ? NOEDGE : NULLEDGE;
					(*wires)[j].val = (*assign_val)[k].val;
				}
			}
			break;
		case ASSIGN_CONST: {
				/* Assign wires inconditionally: */
				for(int j = (int)(assign.wire_off), k = 0; j < (int)(assign.wire_off + assign.wire_len) && j < (int)(wires->size()) && k < (int)(assign.left_side->size()); j++, k++) {
					WireVal logic = assign.left_side_val[k];
					(*wires)[j].edge = logic == _1 ? POSEDGE : logic == _0 ? NEGEDGE : logic == _Z ? NOEDGE : NULLEDGE;
					(*wires)[j].val = (*assign.left_side)[k].val;
				}
			}
			break;
		}
	}

	/* Update signals for this port: */
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
					if(sig.raise == (*signals)[i].raise && sig.edge_trigger == (*signals)[i].edge_trigger) {
						sig_already_called = 1;
						break;
					}
				}
				if(!sig_already_called) {
					sigs_called.push_back((*signals)[i]);
					(*signals)[i].raise(modid, portid, i, (*wires)[i].val, (*wires)[i].edge); /* Call callback if a delta signal is detected */
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

void Port::set_sensitivity_bus(sig_raise_t sig_cback) {
	for(size_t i = 0; i < (*wires).size(); i++)
		set_sensitivity(i, ALLEDGES, sig_cback);
}

void Port::onchange(sig_raise_t sig_cback) {
	set_sensitivity_bus(sig_cback);
}

/* Drive partial part of the wires and try to match the wire_valdrive value in this window: */
PortDriveError Port::drive(uint32_t wire_offset, uint32_t wire_length, std::vector<WireVal> wire_valdrive) {
	if(type != PORT_REG) return PORT_DRIVE_ERROR_NOTAREG;
	if(wire_offset >= (*wires).size()) return PORT_DRIVE_ERROR_OUTOFBOUNDS;

	/* Nullify the undriven wires. Then overwrite undriven wires with driven ones: */
	for(int i = 0; i < (int)(*wires).size(); i++) {
		(*wires)[i].edge = NULLEDGE;
		(*wires)[i].val = _X;
	}

	for(int i = (int)wire_offset, j = (int)wire_valdrive.size() - 1; (i < (int)(*wires).size()) && (i < (int)(wire_offset + wire_length)) && (j >= 0); i++) {
		WireVal logic = wire_valdrive[j--];
		(*wires)[i].edge = logic == _1 ? POSEDGE : logic == _0 ? NEGEDGE : logic == _Z ? NOEDGE : NULLEDGE;
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

void Port::assign(assign_ret_t assign_cback, uint32_t wire_off, uint32_t wire_len, void * cback_args) {
	assign_t new_assign;
	new_assign.assign_func = assign_cback ? assign_cback : 0;
	new_assign.assign_type = ASSIGN_CBACK;
	new_assign.cback_args = cback_args;
	new_assign.condition = 0;
	new_assign.left_side = 0;
	new_assign.right_side = 0;
	new_assign.wire_off = wire_off < wires->size() ? wire_off : 0;
	new_assign.wire_len = wire_len < wires->size() ? wire_len + 1 : new_assign.wire_off + 1 >= wires->size() ? 0 : 1;
	assigns->push_back(new_assign);
}

void Port::assign(assign_ret_t assign_cback, uint32_t wire_off, uint32_t wire_len) {
	assign(assign_cback, wire_off, wire_len, 0);
}

void Port::assign(assign_ret_t assign_cback, uint32_t nth_wire, void * cback_args) {
	assign(assign_cback, nth_wire, 1, cback_args);
}

void Port::assign(assign_ret_t assign_cback, uint32_t nth_wire) {
	assign(assign_cback, nth_wire, 1, 0);
}

void Port::assign(assign_ret_t assign_cback, void * cback_args) {
	assign(assign_cback, 0, wires->size() - 1, cback_args);
}

void Port::assign(assign_ret_t assign_cback) {
	assign(assign_cback, 0, wires->size() - 1, 0);
}

void Port::assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t wire_off,
		uint32_t wire_len, void * cback_args
) {
	std::reverse(left_side.begin(), left_side.end());
	std::reverse(right_side.begin(),right_side.end());

	assign_t new_assign;
	new_assign.assign_func = 0;
	new_assign.assign_type = ASSIGN_COND;
	new_assign.cback_args = cback_args;
	new_assign.condition = condition;
	new_assign.left_side_val = left_side;
	new_assign.left_side = wireval_to_wires(left_side);
	new_assign.right_side_val = right_side;
	new_assign.right_side = wireval_to_wires(right_side);
	new_assign.wire_off = wire_off < wires->size() ? wire_off : 0;
	new_assign.wire_len = wire_len < wires->size() ? wire_len + 1 : new_assign.wire_off + 1 >= wires->size() ? 0 : 1;
	assigns->push_back(new_assign);
}

void Port::assign_cond(uint8_t condition, wireval_t left_side,
		wireval_t right_side, uint32_t wire_off, uint32_t wire_len
) {
	assign_cond(condition, left_side, right_side, wire_off, wire_len, 0);
}

void Port::assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t nth_wire, void * cback_args) {
	assign_cond(condition, left_side, right_side, nth_wire, 1, cback_args);
}

void Port::assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, uint32_t nth_wire) {
	assign_cond(condition, left_side, right_side, nth_wire, 1, 0);
}

void Port::assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side, void * cback_args) {
	assign_cond(condition, left_side, right_side, 0, wires->size() - 1, cback_args);
}

void Port::assign_cond(uint8_t condition, wireval_t left_side, wireval_t right_side) {
	assign_cond(condition, left_side, right_side, 0, wires->size() - 1, 0);
}

void Port::assign_const(wireval_t const_val, uint32_t wire_off, uint32_t wire_len, void * cback_args) {
	std::reverse(const_val.begin(),const_val.end());

	assign_t new_assign;
	new_assign.assign_func = 0;
	new_assign.assign_type = ASSIGN_CONST;
	new_assign.cback_args = cback_args;
	new_assign.condition = 0;
	new_assign.left_side = wireval_to_wires(const_val);
	new_assign.left_side_val = const_val;
	new_assign.right_side = 0;
	new_assign.wire_off = wire_off < wires->size() ? wire_off : 0;
	new_assign.wire_len = wire_len < wires->size() ? wire_len + 1 : new_assign.wire_off + 1 >= wires->size() ? 0 : 1;
	assigns->push_back(new_assign);
}

void Port::assign_const(wireval_t const_val, uint32_t wire_off, uint32_t wire_len) {
	assign_const(const_val, wire_off, wire_len, 0);
}

void Port::assign_const(wireval_t const_val, uint32_t nth_wire) {
	assign_const(const_val, nth_wire, 1, 0);
}

void Port::assign_const(wireval_t const_val, uint32_t nth_wire, void * cback_args) {
	assign_const(const_val, nth_wire, 1, cback_args);
}

void Port::assign_const(wireval_t const_val) {
	assign_const(const_val, 0, wires->size() - 1, 0);
}

void Port::assign_const(wireval_t const_val, void * cback_args) {
	assign_const(const_val, 0, wires->size() - 1, cback_args);
}

void Port::update_assign_condition(uint32_t nth_condition, uint8_t new_cond) {
	if(nth_condition < assigns->size())
		(*assigns)[nth_condition].condition = new_cond;
}

void Port::clean_wires() {
	if(wires_dirty) {
		delete wires;
		wires_dirty = 0;
	}
}

uint32_t Port::get_wire_width() {
	return wires->size();
}

std::vector<wire_t> * Port::get_wires() {
	return wires;
}
