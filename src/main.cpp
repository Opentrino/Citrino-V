#include <iostream>
#include <stdio.h>
#include "citrino.h"
#include "memory.h"
#include "low_lvl/module.h"
#include "low_lvl/port.h"
#include "low_lvl/wireval.h"
#include "low_lvl/refresher.h"

int i = 0;
void cback(uint32_t modid, WireVal wire_logicval, WireEdge edge) {
	std::cout<<"Signal called me:\tid- "<<modid<<", "<<(i++)<<" "<<edge<<"\n";
}

int i2 = 0;
void cback2(uint32_t modid, WireVal wire_logicval, WireEdge edge) {
	std::cout<<"Signal 2 called me:\tid- "<<modid<<", "<<(i2++)<<" "<<edge<<"\n";
}

int i3 = 0;
void cback3(uint32_t modid, WireVal wire_logicval, WireEdge edge) {
	std::cout<<"Signal 3 called me:\tid- "<<modid<<", "<<(i3++)<<" "<<edge<<"\n";
}


class MyComponent : Module {
public:
	Port * out;
	Port * in;
	Port * clk;

	MyComponent() : Module() {
		out = new Port(PORT_OUTPUT, PORT_REG,  32, 0);
		in  = new Port(PORT_INPUT,  PORT_WIRE, 32, 0);
		clk = new Port(PORT_INPUT,  PORT_WIRE, 1,  0);

		out->set_sensitivity(0, ALLEDGES, (sig_raise_t)&cback);
		out->set_sensitivity(1, ALLEDGES, (sig_raise_t)&cback2);

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
			out->drive_all({_Z,_1});
			break;
		case 7:
			out->drive_all({_X,_0});
			break;
		}
		i++;
	}
};

class OtherComponent : Module {
public:
	Port * myport;
	OtherComponent() : Module() {
		myport = new Port(PORT_OUTPUT, PORT_WIRE,  32, 0);
		myport->set_sensitivity(1, ALLEDGES, (sig_raise_t)&cback3);

		addport(myport);
	}

	void update() {

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

	refresher.refresh_all();
	return 0;
}
