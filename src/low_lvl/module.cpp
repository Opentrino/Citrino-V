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
}

Module::~Module() {

}
