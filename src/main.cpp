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

		out->set_sensitivity(0, NEGEDGE, (sig_raise_t)&cback);

		ports.push_back(out);
		ports.push_back(in);
		ports.push_back(clk);
	}

	void update() {

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
