/*
 * citrino.h
 *
 *  Created on: 14/08/2016
 *      Author: Miguel
 */

#ifndef CITRINO_H_
#define CITRINO_H_

#include "low_lvl/module.h"

class Citrino : Module{
public:
	Citrino();
	void update();
	void initial();
};

#endif /* CITRINO_H_ */
