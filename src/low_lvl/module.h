/*
 * module.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <vector>
#include <string>
#include "port.h"

class Module {
public:
	std::string name;
	std::string desc;
	std::string author;
	std::vector<Port*> ports;

	Module();
	virtual ~Module() = 0;
	virtual void update() = 0; /* The module will be updated when its inputs are also updated */
};

#endif /* MODULE_H_ */
