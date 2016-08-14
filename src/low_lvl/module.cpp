/*
 * module.cpp
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#include "module.h"
#include "refresher.h"

Module::Module() {
	Refresher::add_component(this);
	mod_id = Refresher::get_new_module_id();
}

Module::~Module() {

}

void Module::addport(Port & port) {
	ports.push_back(&port);
}

void Module::addport(Port * port) {
	ports.push_back(port);
}
