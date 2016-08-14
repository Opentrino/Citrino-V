#include <iostream>
#include "citrino.h"
#include "low_lvl/module.h"
#include "memory.h"
#include "low_lvl/port.h"
#include "low_lvl/refresher.h"

int i = 0;
void cback(WireVal wire_logicval, WireEdge edge) {
	std::cout<<"Signal called me: "<<(i++)<<" "<<edge<<"\n";
}

class MyComponent : Module {
public:
	Port * out;
	Port * in;
	Port * clk;

	MyComponent() : Module() {
		out = new Port(PORT_OUTPUT, PORT_REG,  2, 0);
		in  = new Port(PORT_INPUT,  PORT_WIRE, 32, 0);
		clk = new Port(PORT_INPUT,  PORT_WIRE, 1,  0);

		out->set_sensitivity(0, ALLEDGES, (sig_raise_t)&cback);

		ports.push_back(out);
		ports.push_back(in);
		ports.push_back(clk);
	}

	int i = 0;
	void update() {
		switch(i) {
		case 2:
			out->drive_all({_1,_0});
			break;
		case 4:
			out->drive_all({_0,_0});
			break;
		case 5:
			out->drive_all({_1,_0});
			break;
		case 6:
			out->drive_all({_Z,_0});
			break;
		case 7:
			out->drive_all({_X,_0});
			break;
		}
		i++;
	}
};

int main() {
	std::cout<<">> Citrino-V Microprocessor\n";
	MainMemory mem;
	Citrino cpu;
	MyComponent c;
	refresher.update();
	return 0;
}
