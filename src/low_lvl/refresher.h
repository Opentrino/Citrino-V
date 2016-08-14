/*
 * refresher.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include <vector>

#include "module.h"

class Refresher { /* Updates the physics of the circuit */
public:
	static std::vector<Module*> modules;
	bool refreshing = 1;

	static void add_component(Module * comp);
	void update();
};

extern Refresher refresher;
