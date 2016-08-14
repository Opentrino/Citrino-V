#include <iostream>
#include <stdio.h>
#include "citrino.h"
#include "memory.h"
#include "low_lvl/module.h"
#include "low_lvl/port.h"
#include "low_lvl/wireval.h"
#include "low_lvl/refresher.h"

class MyComponent : Module {
public:
	Port * out;
	Port * in;
	Port * clk;

	MyComponent() : Module() {
		out = new Port(PORT_OUTPUT, PORT_REG,  32, 0);
		in  = new Port(PORT_INPUT,  PORT_WIRE, 32, 0);
		clk = new Port(PORT_INPUT,  PORT_WIRE, 1,  0);

		addport(out);
		addport(in);
		addport(clk);
	}

	int i = 0;
	void update() {
		switch(i) {
		case 2:
			out->drive_all({_1,_0});
			break;
		case 4:
			out->drive_all({_0,_X});
			break;
		case 5:
			out->drive_all({_1,_0});
			break;
		case 6:
			out->drive_all({_0,_Z,_1});
			break;
		case 7:
			out->drive_all({_X});
			break;
		}
		i++;
	}

	void initial() {

	}
};

void cback(uint32_t modid, uint32_t portid, uint32_t wireid, WireVal wire_logicval, WireEdge edge) {
	uint8_t val = wireval_u8(Refresher::get_wireval(modid, portid, wireid));
	printf("Signal. Module: %d Port: %d Wire: %d Val: %d\n", modid, portid, wireid, val);
}

class OtherComponent : Module {
public:
	Port * myport;

	OtherComponent() : Module() {
		myport = new Port(PORT_OUTPUT, PORT_WIRE,  32, 0);
		myport->set_sensitivity_bus(cback);

		addport(myport);
	}

	void update() {

	}

	void initial() {

	}
};

int main() {
	std::cout<<">> Citrino-V Microprocessor <<\n";
	MainMemory mem;
	Citrino cpu;

	MyComponent c;
	OtherComponent c2;

	/* Connect modules: */
	c.out->connect(c2.myport);

	refresher.init_all();
	refresher.refresh_all();
	return 0;
}
